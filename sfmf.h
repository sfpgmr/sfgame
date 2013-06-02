#pragma once
// Media Foundation headers
//#include <d3d9.h>

namespace sf {


#define VIDEO_WIDTH  1280
#define VIDEO_HEIGHT  720

  extern HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex);
  extern HRESULT WriteFrame(
    IMFSinkWriter *pWriter, 
    DWORD streamIndex, 
    const LONGLONG& rtStart,        // Time stamp.
    const LONGLONG& rtDuration      // Frame duration.
    );
  extern Windows::Foundation::IAsyncActionWithProgress<double>^ WriteAsync(Windows::Storage::Streams::IRandomAccessStream^ stream);

  // Class to start and shutdown Media Foundation
  class AutoMF
  {
  public:
    AutoMF()
      : _bInitialized(false)
    {
      CHK(MFStartup(MF_VERSION));
    }

    ~AutoMF()
    {
      if (_bInitialized)
      {
        (void) MFShutdown();
      }
    }

  private:
    bool _bInitialized;
  };
  ref class  VideoWriter sealed
  {
  public:
    VideoWriter(
      Windows::Storage::Streams::IRandomAccessStream^ stream
      );

    virtual ~VideoWriter()
    {
      Finalize();
    };
  internal:
    virtual void Finalize()
    {
      if(sinkWriter_)
      {
        sinkWriter_->Finalize();
      }
    }
    void WriteSink();
    void WriteTextureToBuffer(ID3D11DeviceContext1Ptr& context ,ID3D11Texture2DPtr& texture);
  private:
    // 出力先ストリームへのポインタ
    Windows::Storage::Streams::IRandomAccessStream^ stream_;
    // 
    IMFSinkWriterPtr sinkWriter_;
    IMFByteStreamPtr byteStream_;
    IMFAttributesPtr attr_;
    IMFMediaTypePtr mediaTypeOut_;
    IMFMediaTypePtr mediaTypeIn_;
    DWORD streamIndex_;
    IMFSamplePtr sample_;
    IMFMediaBufferPtr buffer_;
    LONGLONG sampleTime_;
  };
}
