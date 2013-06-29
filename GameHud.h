//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// GameHud:
// This class maintains the resources necessary to draw the Heads Up Display, which is a
// 2D overlay on top of the main 3D graphics generated for the game.
// The GameHud Render method expects to be called within the context of a D2D BeginDraw
// and makes D2D drawing calls to draw the text elements on the window.

#include "gamemain.h"
#include "DirectXSample.h"
namespace sf {
ref class GameHud
{
internal:
    GameHud();

    void CreateDeviceIndependentResources(
        _In_ IDWriteFactory* dwriteFactory,
        _In_ IWICImagingFactory* wicFactory
        );

    void CreateDeviceResources(_In_ ID2D1DeviceContext* d2dContext);
    void UpdateForWindowSizeChange(_In_ Windows::Foundation::Rect windowBounds);
    void Render(
        _In_ GameMain^ game,
        _In_ ID2D1DeviceContext* d2dContext,
        _In_ Windows::Foundation::Rect windowBounds
        );

    void RenderPause(
      _In_ GameMain^ game,
      _In_ ID2D1DeviceContext* d2dContext,
      _In_ Windows::Foundation::Rect windowBounds
    );


    void DrawScore(
      _In_ GameMain^ game,
      _In_ ID2D1DeviceContext* d2dContext,
      _In_ Windows::Foundation::Rect windowBounds
      );

    void DrawHighScore(
      _In_ GameMain^ game,
      _In_ ID2D1DeviceContext* d2dContext,
      _In_ Windows::Foundation::Rect windowBounds
      );

    void DrawTitle(
      _In_ GameMain^ game,
      _In_ ID2D1DeviceContext* d2dContext,
      _In_ Windows::Foundation::Rect windowBounds
    );

    void DrawHighScores(
      _In_ GameMain^ game,
      _In_ ID2D1DeviceContext* d2dContext,
      _In_ Windows::Foundation::Rect windowBounds
    );

    void DrawPlayerLeft(
      _In_ GameMain^ game,
      _In_ ID2D1DeviceContext* d2dContext,
      _In_ Windows::Foundation::Rect windowBounds
    );

    void DrawGameStart(
      _In_ GameMain^ game,
      _In_ ID2D1DeviceContext* d2dContext,
      _In_ Windows::Foundation::Rect windowBounds
    );

    void DrawGameOver(
      _In_ GameMain^ game,
      _In_ ID2D1DeviceContext* d2dContext,
      _In_ Windows::Foundation::Rect windowBounds
    );

    void SetLicenseInfo(_In_ Platform::String^ licenseInfo);

private:
    Microsoft::WRL::ComPtr<IDWriteFactory>              m_dwriteFactory;
    Microsoft::WRL::ComPtr<IWICImagingFactory>          m_wicFactory;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textBrushGrey;

    Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormatBody;
    //Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormatBodySymbol;
    //Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormatTitleHeader;
    //Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormatTitleBody;
    Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormatTitleLicense;

  /*  Microsoft::WRL::ComPtr<IDWriteTextLayout>           m_titleHeaderLayout;
    Microsoft::WRL::ComPtr<IDWriteTextLayout>           m_titleBodyLayout;*/
    Microsoft::WRL::ComPtr<IDWriteTextLayout>           m_titleLicenseInfoLayout;

    Platform::String^   m_ScoreHeader;
    Platform::String^   m_HighScoreHeader;
    Platform::String^   m_PauseGameString;
    Platform::String^   m_titleLicenseInfo;

    float                                               m_titleBodyVerticalOffset;
    float                                               m_titleLicenseVerticalOffset;
    //D2D1_SIZE_F                                         m_logoSize;
    D2D1_SIZE_F                                         m_maxTitleSize;
};
}
