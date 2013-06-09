//
// SuspensionManager.cpp
// SuspensionManager クラスの実装
//

#include "pch.h"
#include "SuspensionManager.h"

#include <collection.h>
#include <algorithm>

using namespace sfgame::Common;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;

namespace
{
	Map<String^, Object^>^ _sessionState = ref new Map<String^, Object^>();
	String^ sessionStateFilename = "_sessionState.dat";

	// オブジェクトの事前宣言 オブジェクトの読み取り/書き込みサポート
	void WriteObject(Windows::Storage::Streams::DataWriter^ writer, Platform::Object^ object);
	Platform::Object^ ReadObject(Windows::Storage::Streams::DataReader^ reader);
}

/// <summary>
/// 現在のセッションのグローバル セッション状態へのアクセスを提供します。
/// この状態は、<see cref="SaveAsync"/> によってシリアル化され、<see cref="RestoreAsync"/> によって復元されます。
/// また、値は次のいずれかにする必要があります: 整数、単精度浮動小数点、倍精度浮動小数点、
/// ワイド文字、ブール値、文字列および Guid、Map<String^, Object^> (マップ値は同じ制約の影響を受けます) を含むボックス化された値。
/// セッション状態は、できるだけコンパクトになっている必要があります。
/// </summary>
IMap<String^, Object^>^ SuspensionManager::SessionState::get(void)
{
	return _sessionState;
}

/// <summary>
/// コレクションで使用する参照オブジェクトとして WeakReference をラップします。
/// </summary>
private ref class WeakFrame sealed
{
private:
	WeakReference _frameReference;

internal:
	WeakFrame(Frame^ frame) { _frameReference = frame; }
	property Frame^ ResolvedFrame
	{
		Frame^ get(void) { return _frameReference.Resolve<Frame>(); }
	};
};

namespace
{
	std::vector<WeakFrame^> _registeredFrames;
	DependencyProperty^ FrameSessionStateKeyProperty =
		DependencyProperty::RegisterAttached("_FrameSessionStateKeyProperty",
		TypeName(String::typeid), TypeName(SuspensionManager::typeid), nullptr);
	DependencyProperty^ FrameSessionStateProperty =
		DependencyProperty::RegisterAttached("_FrameSessionStateProperty",
		TypeName(IMap<String^, Object^>::typeid), TypeName(SuspensionManager::typeid), nullptr);
}

/// <summary>
/// <see cref="Frame"/> インスタンスを登録し、ナビゲーション履歴を <see cref="SessionState"/> に保存して、
/// ここから復元できるようにします。
/// フレームは、セッション状態管理に参加する場合、作成直後に 1 回登録する必要があります。
/// 登録されしだい、指定されたキーに対して状態が既に復元されていれば、
/// ナビゲーション履歴が直ちに復元されます。
/// また、<see cref="RestoreAsync(String)"/> の後続の呼び出しもナビゲーション履歴を復元します。
/// </summary>
/// <param name="frame">ナビゲーション履歴を管理する必要があるインスタンスです
/// <see cref="SuspensionManager"/></param>
/// <param name="sessionStateKey">ナビゲーション関連情報を格納するのに
/// 使用される <see cref="SessionState"/> への一意キーです。</param>
void SuspensionManager::RegisterFrame(Frame^ frame, String^ sessionStateKey)
{
	if (frame->GetValue(FrameSessionStateKeyProperty) != nullptr)
	{
		throw ref new FailureException("Frames can only be registered to one session state key");
	}

	if (frame->GetValue(FrameSessionStateProperty) != nullptr)
	{
		throw ref new FailureException("Frames must be either be registered before accessing frame session state, or not registered at all");
	}

	// 依存関係プロパティを使用してセッション キーをフレームに関連付け、
	// ナビゲーション状態を管理する必要があるフレームの一覧を保持します
	frame->SetValue(FrameSessionStateKeyProperty, sessionStateKey);
	_registeredFrames.insert(_registeredFrames.begin(), ref new WeakFrame(frame));

	// ナビゲーション状態が復元可能かどうか確認します
	RestoreFrameNavigationState(frame);
}

/// <summary>
/// <see cref="SessionState"/> から <see cref="RegisterFrame"/> によって以前登録された <see cref="Frame"/> の関連付けを解除します。
/// 以前キャプチャされたナビゲーション状態は
/// 削除されます。
/// </summary>
/// <param name="frame">ナビゲーション履歴を管理する必要がなくなった
/// インスタンスです。</param>
void SuspensionManager::UnregisterFrame(Frame^ frame)
{
	// セッション状態を削除し、(到達不能になった弱い参照と共に) ナビゲーション状態が保存される
	// フレームの一覧からフレームを削除します
	auto key = safe_cast<String^>(frame->GetValue(FrameSessionStateKeyProperty));
	if (SessionState->HasKey(key)) SessionState->Remove(key);
	_registeredFrames.erase(
		std::remove_if(_registeredFrames.begin(), _registeredFrames.end(), [=](WeakFrame^& e)
		{
			auto testFrame = e->ResolvedFrame;
			return testFrame == nullptr || testFrame == frame;
		}),
		_registeredFrames.end()
	);
}

/// <summary>
/// 指定された <see cref="Frame"/> に関連付けられているセッション状態のストレージを提供します。
/// <see cref="RegisterFrame"/> で以前登録されたフレームには、
/// グローバルの <see cref="SessionState"/> の一部として自動的に保存および復元されるセッション状態があります。
/// 登録されていないフレームは遷移状態です。
/// 遷移状態は、ナビゲーション キャッシュから破棄されたページを復元する場合に
/// 使用できます。
/// </summary>
/// <remarks>アプリケーションは、フレームのセッション状態を直接処理するのではなく、<see cref="LayoutAwarePage"/> に依存して
/// ページ固有の状態を管理するように選択できます。</remarks>
/// <param name="frame">セッション状態が必要なインスタンスです。</param>
/// <returns><see cref="SessionState"/> と同じシリアル化機構の影響を受ける状態の
/// コレクションです。</returns>
IMap<String^, Object^>^ SuspensionManager::SessionStateForFrame(Frame^ frame)
{
	auto frameState = safe_cast<IMap<String^, Object^>^>(frame->GetValue(FrameSessionStateProperty));

	if (frameState == nullptr)
	{
		auto frameSessionKey = safe_cast<String^>(frame->GetValue(FrameSessionStateKeyProperty));
		if (frameSessionKey != nullptr)
		{
			// 登録されているフレームは、対応するセッション状態を反映します
			if (!_sessionState->HasKey(frameSessionKey))
			{
				_sessionState->Insert(frameSessionKey, ref new Map<String^, Object^>());
			}
			frameState = safe_cast<IMap<String^, Object^>^>(_sessionState->Lookup(frameSessionKey));
		}
		else
		{
			// 登録されていないフレームは遷移状態です
			frameState = ref new Map<String^, Object^>();
		}
		frame->SetValue(FrameSessionStateProperty, frameState);
	}
	return frameState;
}

void SuspensionManager::RestoreFrameNavigationState(Frame^ frame)
{
	auto frameState = SessionStateForFrame(frame);
	if (frameState->HasKey("Navigation"))
	{
		frame->SetNavigationState(safe_cast<String^>(frameState->Lookup("Navigation")));
	}
}

void SuspensionManager::SaveFrameNavigationState(Frame^ frame)
{
	auto frameState = SessionStateForFrame(frame);
	frameState->Insert("Navigation", frame->GetNavigationState());
}

/// <summary>
/// 現在の <see cref="SessionState"/> を保存します。
/// <see cref="RegisterFrame"/> で登録された <see cref="Frame"/> インスタンスは、現在のナビゲーション スタックも保存します。
/// これは、アクティブな <see cref="Page"/> に状態を保存する機会を
/// 順番に提供します。
/// </summary>
/// <returns>セッション状態が保存されたときに反映される非同期タスクです。</returns>
task<void> SuspensionManager::SaveAsync(void)
{
	// 登録されているすべてのフレームのナビゲーション状態を保存します
	for (auto&& weakFrame : _registeredFrames)
	{
		auto frame = weakFrame->ResolvedFrame;
		if (frame != nullptr) SaveFrameNavigationState(frame);
	}

	// セッション状態を同期的にシリアル化して、共有状態への非同期アクセスを
	// 回避します
	auto sessionData = ref new InMemoryRandomAccessStream();
	auto sessionDataWriter = ref new DataWriter(sessionData->GetOutputStreamAt(0));
	WriteObject(sessionDataWriter, _sessionState);

	// セッション状態が同期的にキャプチャされたら、ディスクに結果を書き込む
	// 非同期プロセスが開始します
	return task<unsigned int>(sessionDataWriter->StoreAsync()).then([=](unsigned int)
	{
		return sessionDataWriter->FlushAsync();
	}).then([=](bool flushSucceeded)
	{
		(void)flushSucceeded; // 未使用のパラメーター
		return ApplicationData::Current->LocalFolder->CreateFileAsync(sessionStateFilename,
			CreationCollisionOption::ReplaceExisting);
	}).then([=](StorageFile^ createdFile)
	{
		return createdFile->OpenAsync(FileAccessMode::ReadWrite);
	}).then([=](IRandomAccessStream^ newStream)
	{
		return RandomAccessStream::CopyAndCloseAsync(
			sessionData->GetInputStreamAt(0), newStream->GetOutputStreamAt(0));
	}).then([=](UINT64 copiedBytes)
	{
		(void)copiedBytes; // 未使用のパラメーター
		return;
	});
}

/// <summary>
/// 以前保存された <see cref="SessionState"/> を復元します。
/// <see cref="RegisterFrame"/> で登録された <see cref="Frame"/> インスタンスは、前のナビゲーション状態も復元します。
/// これは、アクティブな <see cref="Page"/> に状態を復元する機会を順番に提供します。
/// ます。
/// </summary>
/// <param name="version">互換性のないセッション状態のバージョンが
/// アプリケーション コードに到達するのを回避するためにセッション状態と比較されるバージョン識別子です。
/// 異なるバージョンで保存された状態は無視され、結果として空の <see cref="SessionState"/> ディクショナリが
/// 作成されます。</param>
/// <returns>セッション状態が読み取られたときに反映される非同期タスクです。
/// このタスクが完了するまで、<see cref="SessionState"/> のコンテンツには
/// 依存できません。</returns>
task<void> SuspensionManager::RestoreAsync(void)
{
	_sessionState->Clear();

	task<StorageFile^> getFileTask(ApplicationData::Current->LocalFolder->GetFileAsync(sessionStateFilename));
	return getFileTask.then([=](StorageFile^ stateFile)
	{
		task<BasicProperties^> getBasicPropertiesTask(stateFile->GetBasicPropertiesAsync());
		return getBasicPropertiesTask.then([=](BasicProperties^ stateFileProperties)
		{
			auto size = unsigned int(stateFileProperties->Size);
			if (size != stateFileProperties->Size) throw ref new FailureException("Session state larger than 4GB");
			task<IRandomAccessStreamWithContentType^> openReadTask(stateFile->OpenReadAsync());
			return openReadTask.then([=](IRandomAccessStreamWithContentType^ stateFileStream)
			{
				auto stateReader = ref new DataReader(stateFileStream);
				return task<unsigned int>(stateReader->LoadAsync(size)).then([=](unsigned int bytesRead)
				{
					(void)bytesRead; // 未使用のパラメーター
					// セッション状態を逆シリアル化します
					Object^ content = ReadObject(stateReader);
					_sessionState = (Map<String^, Object^>^)content;

					// 登録されているフレームを保存された状態に復元します
					for (auto&& weakFrame : _registeredFrames)
					{
						auto frame = weakFrame->ResolvedFrame;
						if (frame != nullptr)
						{
							frame->ClearValue(FrameSessionStateProperty);
							RestoreFrameNavigationState(frame);
						}
					}
				}, task_continuation_context::use_current());
			});
		});
	});
}

#pragma region Object serialization for a known set of types

namespace
{
	// シリアル化された型の特定に使用されるコードです
	enum StreamTypes {
		NullPtrType = 0,

		// サポートされている IPropertyValue 型です
		UInt8Type, UInt16Type, UInt32Type, UInt64Type, Int16Type, Int32Type, Int64Type,
		SingleType, DoubleType, BooleanType, Char16Type, GuidType, StringType,

		// サポートされている追加の型です
		StringToObjectMapType,

		// ストリームの整合性を確保するのに使用されるマーカー値です
		MapEndMarker
	};

	void WriteString(DataWriter^ writer, String^ string)
	{
		writer->WriteByte(StringType);
		writer->WriteUInt32(writer->MeasureString(string));
		writer->WriteString(string);
	}

	void WriteProperty(DataWriter^ writer, IPropertyValue^ propertyValue)
	{
		switch (propertyValue->Type)
		{
		case PropertyType::UInt8:
			writer->WriteByte(UInt8Type);
			writer->WriteByte(propertyValue->GetUInt8());
			return;
		case PropertyType::UInt16:
			writer->WriteByte(UInt16Type);
			writer->WriteUInt16(propertyValue->GetUInt16());
			return;
		case PropertyType::UInt32:
			writer->WriteByte(UInt32Type);
			writer->WriteUInt32(propertyValue->GetUInt32());
			return;
		case PropertyType::UInt64:
			writer->WriteByte(UInt64Type);
			writer->WriteUInt64(propertyValue->GetUInt64());
			return;
		case PropertyType::Int16:
			writer->WriteByte(Int16Type);
			writer->WriteUInt16(propertyValue->GetInt16());
			return;
		case PropertyType::Int32:
			writer->WriteByte(Int32Type);
			writer->WriteUInt32(propertyValue->GetInt32());
			return;
		case PropertyType::Int64:
			writer->WriteByte(Int64Type);
			writer->WriteUInt64(propertyValue->GetInt64());
			return;
		case PropertyType::Single:
			writer->WriteByte(SingleType);
			writer->WriteSingle(propertyValue->GetSingle());
			return;
		case PropertyType::Double:
			writer->WriteByte(DoubleType);
			writer->WriteDouble(propertyValue->GetDouble());
			return;
		case PropertyType::Boolean:
			writer->WriteByte(BooleanType);
			writer->WriteBoolean(propertyValue->GetBoolean());
			return;
		case PropertyType::Char16:
			writer->WriteByte(Char16Type);
			writer->WriteUInt16(propertyValue->GetChar16());
			return;
		case PropertyType::Guid:
			writer->WriteByte(GuidType);
			writer->WriteGuid(propertyValue->GetGuid());
			return;
		case PropertyType::String:
			WriteString(writer, propertyValue->GetString());
			return;
		default:
			throw ref new InvalidArgumentException("Unsupported property type");
		}
	}

	void WriteStringToObjectMap(DataWriter^ writer, IMap<String^, Object^>^ map)
	{
		writer->WriteByte(StringToObjectMapType);
		writer->WriteUInt32(map->Size);
		for (auto&& pair : map)
		{
			WriteObject(writer, pair->Key);
			WriteObject(writer, pair->Value);
		}
		writer->WriteByte(MapEndMarker);
	}

	void WriteObject(DataWriter^ writer, Object^ object)
	{
		if (object == nullptr)
		{
			writer->WriteByte(NullPtrType);
			return;
		}

		auto propertyObject = dynamic_cast<IPropertyValue^>(object);
		if (propertyObject != nullptr)
		{
			WriteProperty(writer, propertyObject);
			return;
		}

		auto mapObject = dynamic_cast<IMap<String^, Object^>^>(object);
		if (mapObject != nullptr)
		{
			WriteStringToObjectMap(writer, mapObject);
			return;
		}

		throw ref new InvalidArgumentException("Unsupported data type");
	}

	String^ ReadString(DataReader^ reader)
	{
		int length = reader->ReadUInt32();
		String^ string = reader->ReadString(length);
		return string;
	}

	IMap<String^, Object^>^ ReadStringToObjectMap(DataReader^ reader)
	{
		auto map = ref new Map<String^, Object^>();
		auto size = reader->ReadUInt32();
		for (unsigned int index = 0; index < size; index++)
		{
			auto key = safe_cast<String^>(ReadObject(reader));
			auto value = ReadObject(reader);
			map->Insert(key, value);
		}
		if (reader->ReadByte() != MapEndMarker)
		{
			throw ref new InvalidArgumentException("Invalid stream");
		}
		return map;
	}

	Object^ ReadObject(DataReader^ reader)
	{
		auto type = reader->ReadByte();
		switch (type)
		{
		case NullPtrType:
			return nullptr;
		case UInt8Type:
			return reader->ReadByte();
		case UInt16Type:
			return reader->ReadUInt16();
		case UInt32Type:
			return reader->ReadUInt32();
		case UInt64Type:
			return reader->ReadUInt64();
		case Int16Type:
			return reader->ReadInt16();
		case Int32Type:
			return reader->ReadInt32();
		case Int64Type:
			return reader->ReadInt64();
		case SingleType:
			return reader->ReadSingle();
		case DoubleType:
			return reader->ReadDouble();
		case BooleanType:
			return reader->ReadBoolean();
		case Char16Type:
			return (char16_t)reader->ReadUInt16();
		case GuidType:
			return reader->ReadGuid();
		case StringType:
			return ReadString(reader);
		case StringToObjectMapType:
			return ReadStringToObjectMap(reader);
		default:
			throw ref new InvalidArgumentException("Unsupported property type");
		}
	}
}

#pragma endregion
