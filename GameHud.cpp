//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "ConstantBuffers.h"
#include "GameHud.h"
#include "GameConstants.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;
using namespace sf;

//----------------------------------------------------------------------

GameHud::GameHud()
{
  m_titleBodyVerticalOffset = GameConstants::Margin;
  //m_logoSize = D2D1::SizeF(0.0f, 0.0f);
  m_ScoreHeader = L"Score";
  m_HighScoreHeader = L"High Score";
  m_PauseGameString = L"Pause Game";
}

//----------------------------------------------------------------------

void GameHud::CreateDeviceIndependentResources(
  _In_ IDWriteFactory* dwriteFactory,
  _In_ IWICImagingFactory* wicFactory
  )
{
  m_dwriteFactory = dwriteFactory;
  m_wicFactory = wicFactory;

  ThrowIfFailed(
    m_dwriteFactory->CreateTextFormat(
    L"Segoe UI",
    nullptr,
    DWRITE_FONT_WEIGHT_LIGHT,
    DWRITE_FONT_STYLE_NORMAL,
    DWRITE_FONT_STRETCH_NORMAL,
    GameConstants::HudBodyPointSize,
    L"en-us",
    &m_textFormatBody
    )
    );
  /* ThrowIfFailed(
  m_dwriteFactory->CreateTextFormat(
  L"Segoe UI Symbol",
  nullptr,
  DWRITE_FONT_WEIGHT_LIGHT,
  DWRITE_FONT_STYLE_NORMAL,
  DWRITE_FONT_STRETCH_NORMAL,
  GameConstants::HudBodyPointSize,
  L"en-us",
  &m_textFormatBodySymbol
  )
  );
  ThrowIfFailed(
  m_dwriteFactory->CreateTextFormat(
  L"Segoe UI Light",
  nullptr,
  DWRITE_FONT_WEIGHT_LIGHT,
  DWRITE_FONT_STYLE_NORMAL,
  DWRITE_FONT_STRETCH_NORMAL,
  GameConstants::HudTitleHeaderPointSize,
  L"en-us",
  &m_textFormatTitleHeader
  )
  );
  ThrowIfFailed(
  m_dwriteFactory->CreateTextFormat(
  L"Segoe UI Light",
  nullptr,
  DWRITE_FONT_WEIGHT_LIGHT,
  DWRITE_FONT_STYLE_NORMAL,
  DWRITE_FONT_STRETCH_NORMAL,
  GameConstants::HudTitleBodyPointSize,
  L"en-us",
  &m_textFormatTitleBody
  )
  );*/
  ThrowIfFailed(
    m_dwriteFactory->CreateTextFormat(
    L"Segoe UI Bold",
    nullptr,
    DWRITE_FONT_WEIGHT_BOLD,
    DWRITE_FONT_STYLE_NORMAL,
    DWRITE_FONT_STRETCH_NORMAL,
    GameConstants::HudLicensePointSize,
    L"en-us",
    &m_textFormatTitleLicense
    )
    );

  ThrowIfFailed(m_textFormatBody->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
  ThrowIfFailed(m_textFormatBody->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
  //ThrowIfFailed(m_textFormatBodySymbol->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
  //ThrowIfFailed(m_textFormatBodySymbol->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
  //ThrowIfFailed(m_textFormatTitleHeader->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
  //ThrowIfFailed(m_textFormatTitleHeader->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
  //ThrowIfFailed(m_textFormatTitleBody->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
  //ThrowIfFailed(m_textFormatTitleBody->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
  ThrowIfFailed(m_textFormatTitleLicense->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
  ThrowIfFailed(m_textFormatTitleLicense->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
}

//----------------------------------------------------------------------

void GameHud::CreateDeviceResources(_In_ ID2D1DeviceContext* d2dContext)
{
  //auto location = Package::Current->InstalledLocation;
  //Platform::String^ path = Platform::String::Concat(location->Path, "\\");
  //path = Platform::String::Concat(path, "windows-sdk.png");

  //ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
  //ThrowIfFailed(
  //  m_wicFactory->CreateDecoderFromFilename(
  //  path->Data(),
  //  nullptr,
  //  GENERIC_READ,
  //  WICDecodeMetadataCacheOnDemand,
  //  &wicBitmapDecoder
  //  )
  //  );

  //ComPtr<IWICBitmapFrameDecode> wicBitmapFrame;
  //ThrowIfFailed(
  //  wicBitmapDecoder->GetFrame(0, &wicBitmapFrame)
  //  );

  //ComPtr<IWICFormatConverter> wicFormatConverter;
  //ThrowIfFailed(
  //  m_wicFactory->CreateFormatConverter(&wicFormatConverter)
  //  );

  //ThrowIfFailed(
  //  wicFormatConverter->Initialize(
  //  wicBitmapFrame.Get(),
  //  GUID_WICPixelFormat32bppPBGRA,
  //  WICBitmapDitherTypeNone,
  //  nullptr,
  //  0.0,
  //  WICBitmapPaletteTypeCustom  // The BGRA format has no palette so this value is ignored.
  //  )
  //  );

  //double dpiX = 96.0f;
  //double dpiY = 96.0f;
  //ThrowIfFailed(
  //  wicFormatConverter->GetResolution(&dpiX, &dpiY)
  //  );

  //// Create D2D Resources
  //ThrowIfFailed(
  //  d2dContext->CreateBitmapFromWicBitmap(
  //  wicFormatConverter.Get(),
  //  BitmapProperties(
  //  PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
  //  static_cast<float>(dpiX),
  //  static_cast<float>(dpiY)
  //  ),
  //  &m_logoBitmap
  //  )
  //  );

  //m_logoSize = m_logoBitmap->GetSize();

  ThrowIfFailed(
    d2dContext->CreateSolidColorBrush(
    D2D1::ColorF(D2D1::ColorF::White),
    &m_textBrush
    )
    );
  ThrowIfFailed(
    d2dContext->CreateSolidColorBrush(
    D2D1::ColorF(0xaaaaaa, 1.0f),
    &m_textBrushGrey
    )
    );
}

//----------------------------------------------------------------------

void GameHud::UpdateForWindowSizeChange(_In_ Windows::Foundation::Rect windowBounds)
{
  m_maxTitleSize.width = windowBounds.Width - GameConstants::HudSafeWidth;
  m_maxTitleSize.height = windowBounds.Height;

  float headerWidth = m_maxTitleSize.width - (2 * GameConstants::Margin);

  if (headerWidth > 0)
  {

    //ThrowIfFailed(
    //  m_dwriteFactory->CreateTextLayout(
    //  m_titleHeader->Data(),
    //  m_titleHeader->Length(),
    //  m_textFormatTitleHeader.Get(),
    //  headerWidth,
    //  m_maxTitleSize.height,
    //  &m_titleHeaderLayout
    //  )
    //  );

    //DWRITE_TEXT_METRICS metrics = {0};
    //ThrowIfFailed(
    //  m_titleHeaderLayout->GetMetrics(&metrics)
    //  );

    //// Compute the vertical size of the laid out Header and logo.  This could change
    //// based on the window size and the layout of the text.  In some cases the text
    //// may wrap.
    //m_titleBodyVerticalOffset = max(m_logoSize.height + GameConstants::Margin * 2, metrics.height + 2 * GameConstants::Margin);

    //ThrowIfFailed(
    //  m_dwriteFactory->CreateTextLayout(
    //  m_titleBody->Data(),
    //  m_titleBody->Length(),
    //  m_textFormatTitleBody.Get(),
    //  m_maxTitleSize.width,
    //  m_maxTitleSize.height - m_titleBodyVerticalOffset,
    //  &m_titleBodyLayout
    //  )
    //  );

    //ThrowIfFailed(
    //  m_titleBodyLayout->GetMetrics(&metrics)
    //  );

    //m_titleLicenseVerticalOffset = m_titleBodyVerticalOffset + metrics.height + GameConstants::Margin;

    //ThrowIfFailed(
    //  m_dwriteFactory->CreateTextLayout(
    //  m_titleLicenseInfo->Data(),
    //  m_titleLicenseInfo->Length(),
    //  m_textFormatTitleLicense.Get(),
    //  m_maxTitleSize.width,
    //  m_maxTitleSize.height - m_titleLicenseVerticalOffset,
    //  &m_titleLicenseInfoLayout
    //  )
    //  );
  }
}

//----------------------------------------------------------------------

void GameHud::SetLicenseInfo(_In_ Platform::String^ licenseInfo)
{
  m_titleLicenseInfo = licenseInfo;

  ThrowIfFailed(
    m_dwriteFactory->CreateTextLayout(
    m_titleLicenseInfo->Data(),
    m_titleLicenseInfo->Length(),
    m_textFormatTitleLicense.Get(),
    m_maxTitleSize.width,
    m_maxTitleSize.height - m_titleLicenseVerticalOffset,
    &m_titleLicenseInfoLayout
    )
    );
}

//----------------------------------------------------------------------

void GameHud::Render(
  _In_ GameMain^ game,
  _In_ ID2D1DeviceContext* d2dContext,
  _In_ Windows::Foundation::Rect windowBounds
  )
{
  //if (m_showTitle)
  //{
  //  d2dContext->DrawBitmap(
  //    m_logoBitmap.Get(),
  //    D2D1::RectF(
  //    GameConstants::Margin,
  //    GameConstants::Margin,
  //    m_logoSize.width + GameConstants::Margin,
  //    m_logoSize.height + GameConstants::Margin
  //    )
  //    );
  //  d2dContext->DrawTextLayout(
  //    Point2F(m_logoSize.width + 2.0f * GameConstants::Margin, GameConstants::Margin),
  //    m_titleHeaderLayout.Get(),
  //    m_textBrush.Get()
  //    );
  //  d2dContext->DrawTextLayout(
  //    Point2F(GameConstants::Margin, m_titleBodyVerticalOffset),
  //    m_titleBodyLayout.Get(),
  //    m_textBrush.Get()
  //    );
  //  d2dContext->DrawTextLayout(
  //    Point2F(GameConstants::Margin, m_titleLicenseVerticalOffset),
  //    m_titleLicenseInfoLayout.Get(),
  //    m_textBrush.Get()
  //    );
  //}

  //int length = swprintf_s(
  //    wsbuffer,
  //    bufferLength,
  //    L"Hits:\t%10d\nShots:\t%10d\nTime:\t%8.1f",
  //    game->TotalHits(),
  //    game->TotalShots(),
  //    game->TimeRemaining()
  //    );

  std::wstring out((boost::wformat(L"Time:%04.08f") % game->processTime()).str());
  d2dContext->DrawText(
    out.data(),
    out.size(),
    m_textFormatBody.Get(),
    D2D1::RectF(
    windowBounds.Width - GameConstants::HudRightOffset,
    GameConstants::HudTopOffset,
    windowBounds.Width,
    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize + GameConstants::Margin) * 3
    ),
    m_textBrush.Get()
    );

  DrawScore(game,d2dContext,windowBounds);
  DrawHighScore(game,d2dContext,windowBounds);
  DrawPlayerLeft(game,d2dContext,windowBounds);



  // Using the unicode characters starting at 0x2780 ( ➀ ) for the consecutive levels of the game.
  // For completed levels start with 0x278A ( ➊ ) (This is 0x2780 + 10).
  //uint32 levelCharacter[6];
  //for (uint32 i = 0; i < 6; i++)
  //{
  //  levelCharacter[i] = 0x2780 + i + ((static_cast<uint32>(game->LevelCompleted()) == i) ? 10 : 0);
  //}

  //if (game->IsTrial())
  //{
  //  length = swprintf_s(
  //    wsbuffer,
  //    bufferLength,
  //    L"%lc %lc",
  //    levelCharacter[0],
  //    levelCharacter[1]
  //  );
  //  d2dContext->DrawText(
  //    wsbuffer,
  //    length,
  //    m_textFormatBodySymbol.Get(),
  //    D2D1::RectF(
  //    windowBounds.Width - GameConstants::HudRightOffset,
  //    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize + GameConstants::Margin) * 3 + GameConstants::Margin,
  //    windowBounds.Width,
  //    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize+ GameConstants::Margin) * 4
  //    ),
  //    m_textBrush.Get()
  //    );

  //  length = swprintf_s(
  //    wsbuffer,
  //    bufferLength,
  //    L"%lc %lc %lc %lc",
  //    levelCharacter[2],
  //    levelCharacter[3],
  //    levelCharacter[4],
  //    levelCharacter[5]
  //  );

  //  d2dContext->DrawText(
  //    wsbuffer,
  //    length,
  //    m_textFormatBodySymbol.Get(),
  //    D2D1::RectF(
  //    windowBounds.Width - GameConstants::HudRightOffset + 52.0f,
  //    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize + GameConstants::Margin) * 3 + GameConstants::Margin,
  //    windowBounds.Width,
  //    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize+ GameConstants::Margin) * 4
  //    ),
  //    m_textBrushGrey.Get()
  //    );
  //}
  //else
  //{
  //  length = swprintf_s(
  //    wsbuffer,
  //    bufferLength,
  //    L"%lc %lc %lc %lc %lc %lc",
  //    levelCharacter[0],
  //    levelCharacter[1],
  //    levelCharacter[2],
  //    levelCharacter[3],
  //    levelCharacter[4],
  //    levelCharacter[5]
  //  );
  //  d2dContext->DrawText(
  //    wsbuffer,
  //    length,
  //    m_textFormatBodySymbol.Get(),
  //    D2D1::RectF(
  //    windowBounds.Width - GameConstants::HudRightOffset,
  //    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize + GameConstants::Margin) * 3 + GameConstants::Margin,
  //    windowBounds.Width,
  //    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize+ GameConstants::Margin) * 4
  //    ),
  //    m_textBrush.Get()
  //    );
  //}

  // Draw a rectangle for the touch input for the move control.
  d2dContext->DrawRectangle(
    D2D1::RectF(
    0.0f,
    windowBounds.Height - GameConstants::TouchRectangleSize,
    GameConstants::TouchRectangleSize,
    windowBounds.Height
    ),
    m_textBrush.Get()
    );
  // Draw a rectangle for the touch input for the fire control.
  d2dContext->DrawRectangle(
    D2D1::RectF(
    windowBounds.Width - GameConstants::TouchRectangleSize,
    windowBounds.Height - GameConstants::TouchRectangleSize,
    windowBounds.Width,
    windowBounds.Height
    ),
    m_textBrush.Get()
    );

  // Draw the cross hairs
  d2dContext->DrawLine(
    D2D1::Point2F(windowBounds.Width / 2.0f - GameConstants::CrossHairHalfSize, windowBounds.Height / 2.0f),
    D2D1::Point2F(windowBounds.Width / 2.0f + GameConstants::CrossHairHalfSize, windowBounds.Height / 2.0f),
    m_textBrush.Get(),
    3.0f
    );
  d2dContext->DrawLine(
    D2D1::Point2F(windowBounds.Width / 2.0f, windowBounds.Height / 2.0f - GameConstants::CrossHairHalfSize),
    D2D1::Point2F(windowBounds.Width / 2.0f, windowBounds.Height / 2.0f + GameConstants::CrossHairHalfSize),
    m_textBrush.Get(),
    3.0f
    );
}
//----------------------------------------------------------------------

void GameHud::RenderPause(
  _In_ GameMain^ game,
  _In_ ID2D1DeviceContext* d2dContext,
  _In_ Windows::Foundation::Rect windowBounds
  )
{
  // This section is only used after the game state has been initialized.

  std::wstring out(L"Pause");
  d2dContext->DrawText(
    out.data(),
    out.size(),
    m_textFormatBody.Get(),
    D2D1::RectF(
    windowBounds.Width - GameConstants::HudRightOffset,
    GameConstants::HudTopOffset,
    windowBounds.Width,
    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize + GameConstants::Margin) * 3
    ),
    m_textBrush.Get()
    );
}



void GameHud::DrawScore(
  _In_ GameMain^ game,
  _In_ ID2D1DeviceContext* d2dContext,
  _In_ Windows::Foundation::Rect windowBounds
  )
{
  D2D1_RECT_F r = {
    GameConstants::Margin * 10,GameConstants::HudTopOffset
    ,windowBounds.Width -  GameConstants::Margin * 10
    ,windowBounds.Height - GameConstants::HudTopOffset
  };
  d2dContext->DrawText(
    m_ScoreHeader->Data(),m_ScoreHeader->Length(),m_textFormatBody.Get()
    ,r,
    m_textBrush.Get()
    );

  r.top += GameConstants::Margin * 6;

  std::wstring score((boost::wformat(L"%08d") % game->score).str());
  d2dContext->DrawText(
    score.data(),score.length(),m_textFormatBody.Get()
    ,r,
    m_textBrush.Get()
    );
}

void GameHud::DrawHighScore(
  _In_ GameMain^ game,
  _In_ ID2D1DeviceContext* d2dContext,
  _In_ Windows::Foundation::Rect windowBounds
  )
{
  D2D1_RECT_F r = {
    GameConstants::Margin * 70,GameConstants::HudTopOffset
    ,windowBounds.Width -  GameConstants::Margin * 70
    ,windowBounds.Height - GameConstants::HudTopOffset
  };
  d2dContext->DrawText(
    m_HighScoreHeader->Data(),m_HighScoreHeader->Length(),m_textFormatBody.Get()
    ,r,
    m_textBrush.Get()
    );

  r.top += GameConstants::Margin * 6;

  std::wstring score((boost::wformat(L"%08d") % game->highScoreEntries.at(0).Score).str());
  d2dContext->DrawText(
    score.data(),score.length(),m_textFormatBody.Get()
    ,r,
    m_textBrush.Get()
    );

}

const std::wstring playerLeftTitle(L"Player Left");

void GameHud::DrawPlayerLeft(
  _In_ GameMain^ game,
  _In_ ID2D1DeviceContext* d2dContext,
  _In_ Windows::Foundation::Rect windowBounds
  )
{
  D2D1_RECT_F r = {
    GameConstants::Margin * 140,GameConstants::HudTopOffset
    ,windowBounds.Width 
    ,windowBounds.Height - GameConstants::HudTopOffset
  };
  d2dContext->DrawText(
    playerLeftTitle.data(),playerLeftTitle.size(),m_textFormatBody.Get()
    ,r,
    m_textBrush.Get()
    );

  r.top += GameConstants::Margin * 6;

  std::wstring playerLeft((boost::wformat(L"%02d") % game->playerLeft).str());
  d2dContext->DrawText(
    playerLeft.data(),playerLeft.length(),m_textFormatBody.Get()
    ,r,
    m_textBrush.Get()
    );

}

void GameHud::DrawTitle(
  _In_ GameMain^ game,
  _In_ ID2D1DeviceContext* d2dContext,
  _In_ Windows::Foundation::Rect windowBounds
  )
{
  std::wstring out(L"Title");
  d2dContext->DrawText(
    out.data(),
    out.size(),
    m_textFormatBody.Get(),
    D2D1::RectF(
    windowBounds.Width - GameConstants::HudRightOffset,
    GameConstants::HudTopOffset,
    windowBounds.Width,
    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize + GameConstants::Margin) * 3
    ),
    m_textBrush.Get()
    );
}

void GameHud::DrawHighScores(
  _In_ GameMain^ game,
  _In_ ID2D1DeviceContext* d2dContext,
  _In_ Windows::Foundation::Rect windowBounds
  )
{
  std::wstring out(L"Title");
  d2dContext->DrawText(
    out.data(),
    out.size(),
    m_textFormatBody.Get(),
    D2D1::RectF(
    windowBounds.Width - GameConstants::HudRightOffset,
    GameConstants::HudTopOffset,
    windowBounds.Width,
    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize + GameConstants::Margin) * 3
    ),
    m_textBrush.Get()
    );
}

void GameHud::DrawGameStart(
  _In_ GameMain^ game,
  _In_ ID2D1DeviceContext* d2dContext,
  _In_ Windows::Foundation::Rect windowBounds
  )
{
  std::wstring out(L"Game Start");
  d2dContext->DrawText(
    out.data(),
    out.size(),
    m_textFormatBody.Get(),
    D2D1::RectF(
    windowBounds.Width - GameConstants::HudRightOffset,
    GameConstants::HudTopOffset,
    windowBounds.Width,
    GameConstants::HudTopOffset + (GameConstants::HudBodyPointSize + GameConstants::Margin) * 3
    ),
    m_textBrush.Get()
    );
}

void GameHud::DrawGameOver(
  _In_ GameMain^ game,
  _In_ ID2D1DeviceContext* d2dContext,
  _In_ Windows::Foundation::Rect windowBounds
  )
{
  std::wstring out(L"Game Over");
  d2dContext->DrawText(
    out.data(),
    out.size(),
    m_textFormatBody.Get(),
    D2D1::RectF(
    windowBounds.Width / 2.0f,
    windowBounds.Height / 2.0f,
    windowBounds.Width,
    windowBounds.Height
    ),
    m_textBrush.Get()
    );
}


