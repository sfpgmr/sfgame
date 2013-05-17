//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "DirectXSample.h"

#define _WRL_PTR_TYPEDEF(x) typedef Microsoft::WRL::ComPtr<x> x ## Ptr



// Direct Write

_WRL_PTR_TYPEDEF(IDWriteFactory);
_WRL_PTR_TYPEDEF(IDWriteFactory1);
_WRL_PTR_TYPEDEF(IDWriteGdiInterop);
_WRL_PTR_TYPEDEF(IDWriteFontFace1);
_WRL_PTR_TYPEDEF(IDWriteFontFace);
_WRL_PTR_TYPEDEF(IDWriteFont1);
_WRL_PTR_TYPEDEF(IDWriteFont);
_WRL_PTR_TYPEDEF(IDWriteFontFamily);
_WRL_PTR_TYPEDEF(IDWriteFontCollection);
_WRL_PTR_TYPEDEF(IDWriteLocalizedStrings);
_WRL_PTR_TYPEDEF(IDWriteTextFormat);
_WRL_PTR_TYPEDEF(IDWriteTextAnalyzer1);
_WRL_PTR_TYPEDEF(IDWriteTextLayout);
_WRL_PTR_TYPEDEF(IDWriteTextLayout1);
_WRL_PTR_TYPEDEF(IDWriteFontFile);

// Direct2D

_WRL_PTR_TYPEDEF(ID2D1AnalysisTransform);
_WRL_PTR_TYPEDEF(ID2D1Bitmap);
_WRL_PTR_TYPEDEF(ID2D1Bitmap1);
_WRL_PTR_TYPEDEF(ID2D1BitmapBrush);
_WRL_PTR_TYPEDEF(ID2D1BitmapBrush1);
_WRL_PTR_TYPEDEF(ID2D1BitmapRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1BlendTransform);
_WRL_PTR_TYPEDEF(ID2D1BorderTransform);
_WRL_PTR_TYPEDEF(ID2D1BoundsAdjustmentTransform);
_WRL_PTR_TYPEDEF(ID2D1Brush);
_WRL_PTR_TYPEDEF(ID2D1ColorContext);
_WRL_PTR_TYPEDEF(ID2D1CommandList);
_WRL_PTR_TYPEDEF(ID2D1CommandSink);
_WRL_PTR_TYPEDEF(ID2D1ComputeInfo);
_WRL_PTR_TYPEDEF(ID2D1ComputeTransform);
_WRL_PTR_TYPEDEF(ID2D1ConcreteTransform);
_WRL_PTR_TYPEDEF(ID2D1DCRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Device);
_WRL_PTR_TYPEDEF(ID2D1DeviceContext);
_WRL_PTR_TYPEDEF(ID2D1DrawInfo);
_WRL_PTR_TYPEDEF(ID2D1DrawingStateBlock);
_WRL_PTR_TYPEDEF(ID2D1DrawingStateBlock1);
_WRL_PTR_TYPEDEF(ID2D1DrawTransform);
_WRL_PTR_TYPEDEF(ID2D1Effect);
_WRL_PTR_TYPEDEF(ID2D1EffectContext);
_WRL_PTR_TYPEDEF(ID2D1EffectImpl);
_WRL_PTR_TYPEDEF(ID2D1EllipseGeometry);
_WRL_PTR_TYPEDEF(ID2D1Factory);
_WRL_PTR_TYPEDEF(ID2D1Factory1);
//_WRL_PTR_TYPEDEF(ID2D1GdiInteropRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1GdiMetafile);
_WRL_PTR_TYPEDEF(ID2D1GdiMetafileSink);
_WRL_PTR_TYPEDEF(ID2D1Geometry);
_WRL_PTR_TYPEDEF(ID2D1GeometryGroup);
_WRL_PTR_TYPEDEF(ID2D1GeometrySink);
_WRL_PTR_TYPEDEF(ID2D1GradientStopCollection);
_WRL_PTR_TYPEDEF(ID2D1GradientStopCollection1);
_WRL_PTR_TYPEDEF(ID2D1HwndRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Image);
_WRL_PTR_TYPEDEF(ID2D1ImageBrush);
_WRL_PTR_TYPEDEF(ID2D1Layer);
_WRL_PTR_TYPEDEF(ID2D1LinearGradientBrush);
_WRL_PTR_TYPEDEF(ID2D1Mesh);
//_WRL_PTR_TYPEDEF(ID2D1MultiThread);
_WRL_PTR_TYPEDEF(ID2D1OffsetTransform);
_WRL_PTR_TYPEDEF(ID2D1PathGeometry);
_WRL_PTR_TYPEDEF(ID2D1PathGeometry1);
_WRL_PTR_TYPEDEF(ID2D1PrintControl);
_WRL_PTR_TYPEDEF(ID2D1Properties);
_WRL_PTR_TYPEDEF(ID2D1RadialGradientBrush);
_WRL_PTR_TYPEDEF(ID2D1RectangleGeometry);
_WRL_PTR_TYPEDEF(ID2D1RenderInfo);
_WRL_PTR_TYPEDEF(ID2D1RenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Resource);
_WRL_PTR_TYPEDEF(ID2D1ResourceTexture);
_WRL_PTR_TYPEDEF(ID2D1RoundedRectangleGeometry);
_WRL_PTR_TYPEDEF(ID2D1SimplifiedGeometrySink);
_WRL_PTR_TYPEDEF(ID2D1SolidColorBrush);
_WRL_PTR_TYPEDEF(ID2D1SourceTransform);
_WRL_PTR_TYPEDEF(ID2D1StrokeStyle);
_WRL_PTR_TYPEDEF(ID2D1StrokeStyle1);
_WRL_PTR_TYPEDEF(ID2D1TessellationSink);
_WRL_PTR_TYPEDEF(ID2D1Transform);
_WRL_PTR_TYPEDEF(ID2D1TransformedGeometry);
_WRL_PTR_TYPEDEF(ID2D1TransformGraph);
_WRL_PTR_TYPEDEF(ID2D1TransformNode);
_WRL_PTR_TYPEDEF(ID2D1VertexBuffer);

// WIC

_WRL_PTR_TYPEDEF(IWICImagingFactory);
_WRL_PTR_TYPEDEF(IWICBitmapDecoder);
_WRL_PTR_TYPEDEF(IWICBitmapFrameDecode);
_WRL_PTR_TYPEDEF(IWICStream);
_WRL_PTR_TYPEDEF(IWICFormatConverter);
_WRL_PTR_TYPEDEF(IWICBitmapScaler);
//_WRL_PTR_TYPEDEF(ITaskbarList3);

// DXGI 

_WRL_PTR_TYPEDEF(IDXGIAdapter);
_WRL_PTR_TYPEDEF(IDXGIAdapter1);
_WRL_PTR_TYPEDEF(IDXGIAdapter2);
//_WRL_PTR_TYPEDEF(IDXGIDebug);
_WRL_PTR_TYPEDEF(IDXGIDevice);
_WRL_PTR_TYPEDEF(IDXGIDevice1);
_WRL_PTR_TYPEDEF(IDXGIDevice2);
_WRL_PTR_TYPEDEF(IDXGIDeviceSubObject);
_WRL_PTR_TYPEDEF(IDXGIDisplayControl);
_WRL_PTR_TYPEDEF(IDXGIFactory);
_WRL_PTR_TYPEDEF(IDXGIFactory1);
_WRL_PTR_TYPEDEF(IDXGIFactory2);
//_WRL_PTR_TYPEDEF(IDXGIInfoQueue);
_WRL_PTR_TYPEDEF(IDXGIKeyedMutex);
_WRL_PTR_TYPEDEF(IDXGIObject);
_WRL_PTR_TYPEDEF(IDXGIOutput);
_WRL_PTR_TYPEDEF(IDXGIOutput1);
_WRL_PTR_TYPEDEF(IDXGIOutputDuplication);
_WRL_PTR_TYPEDEF(IDXGIResource);
_WRL_PTR_TYPEDEF(IDXGIResource1);
_WRL_PTR_TYPEDEF(IDXGISurface);
_WRL_PTR_TYPEDEF(IDXGISurface1);
_WRL_PTR_TYPEDEF(IDXGISurface2);
_WRL_PTR_TYPEDEF(IDXGISwapChain);
_WRL_PTR_TYPEDEF(IDXGISwapChain1);

// Direct3D

_WRL_PTR_TYPEDEF(ID3D11Asynchronous);
_WRL_PTR_TYPEDEF(ID3D11BlendState);
_WRL_PTR_TYPEDEF(ID3D11BlendState1);
_WRL_PTR_TYPEDEF(ID3D11CommandList);
_WRL_PTR_TYPEDEF(ID3D11Counter);
_WRL_PTR_TYPEDEF(ID3D11DepthStencilState);
_WRL_PTR_TYPEDEF(ID3D11Device);
_WRL_PTR_TYPEDEF(ID3D11Device1);
_WRL_PTR_TYPEDEF(ID3D11DeviceChild);
_WRL_PTR_TYPEDEF(ID3D11DeviceContext);
_WRL_PTR_TYPEDEF(ID3D11DeviceContext1);
_WRL_PTR_TYPEDEF(ID3DDeviceContextState);
_WRL_PTR_TYPEDEF(ID3D11InputLayout);
_WRL_PTR_TYPEDEF(ID3D11Predicate);
_WRL_PTR_TYPEDEF(ID3D11Query);
_WRL_PTR_TYPEDEF(ID3D11RasterizerState);
_WRL_PTR_TYPEDEF(ID3D11RasterizerState1);
_WRL_PTR_TYPEDEF(ID3D11SamplerState);
_WRL_PTR_TYPEDEF(ID3D11Debug);
_WRL_PTR_TYPEDEF(ID3D11InfoQueue);
_WRL_PTR_TYPEDEF(ID3D11RefDefaultTrackingOptions);
_WRL_PTR_TYPEDEF(ID3D11RefTrackingOptions);
_WRL_PTR_TYPEDEF(ID3D11SwitchToRef);
_WRL_PTR_TYPEDEF(ID3D11TracingDevice);
_WRL_PTR_TYPEDEF(ID3D11Buffer);
_WRL_PTR_TYPEDEF(ID3D11DepthStencilView);
_WRL_PTR_TYPEDEF(ID3D11RenderTargetView);
_WRL_PTR_TYPEDEF(ID3D11Resource);
_WRL_PTR_TYPEDEF(ID3D11ShaderResourceView);
_WRL_PTR_TYPEDEF(ID3D11Texture1D);
_WRL_PTR_TYPEDEF(ID3D11Texture2D);
_WRL_PTR_TYPEDEF(ID3D11Texture3D);
_WRL_PTR_TYPEDEF(ID3D11UnorderedAccessView);
_WRL_PTR_TYPEDEF(ID3D11View);
_WRL_PTR_TYPEDEF(ID3D11ClassInstance);
_WRL_PTR_TYPEDEF(ID3D11ClassLinkage);
_WRL_PTR_TYPEDEF(ID3D11ComputeShader);
_WRL_PTR_TYPEDEF(ID3D11DomainShader);
_WRL_PTR_TYPEDEF(ID3D11GeometryShader);
_WRL_PTR_TYPEDEF(ID3D11HullShader);
_WRL_PTR_TYPEDEF(ID3D11PixelShader);
//_WRL_PTR_TYPEDEF(ID3D11ShaderReflection);
//_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionConstantBuffer);
//_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionType);
//_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionVariable);
//_WRL_PTR_TYPEDEF(ID3D11ShaderTrace);
//_WRL_PTR_TYPEDEF(ID3D11ShaderTraceFactory);
_WRL_PTR_TYPEDEF(ID3D11VertexShader);
_WRL_PTR_TYPEDEF(ID3DBlob);
_WRL_PTR_TYPEDEF(ID3DInclude);
_WRL_PTR_TYPEDEF(ID3DUserDefinedAnnotation);

// DirectComposition

////_WRL_PTR_TYPEDEF(IDCompositionClip);
//_WRL_PTR_TYPEDEF(IDCompositionDevice);
//_WRL_PTR_TYPEDEF(IDCompositionEffect);
//_WRL_PTR_TYPEDEF(IDCompositionEffectGroup);
//_WRL_PTR_TYPEDEF(IDCompositionMatrixTransform);
//_WRL_PTR_TYPEDEF(IDCompositionMatrixTransform3D);
//_WRL_PTR_TYPEDEF(IDCompositionRectangleClip);
//_WRL_PTR_TYPEDEF(IDCompositionRotateTransform);
//_WRL_PTR_TYPEDEF(IDCompositionRotateTransform3D);
//_WRL_PTR_TYPEDEF(IDCompositionScaleTransform);
//_WRL_PTR_TYPEDEF(IDCompositionScaleTransform3D);
//_WRL_PTR_TYPEDEF(IDCompositionSkewTransform);
//_WRL_PTR_TYPEDEF(IDCompositionSurface);
//_WRL_PTR_TYPEDEF(IDCompositionTarget);
//_WRL_PTR_TYPEDEF(IDCompositionTransform);
//_WRL_PTR_TYPEDEF(IDCompositionTransform3D);
//_WRL_PTR_TYPEDEF(IDCompositionTranslateTransform);
//_WRL_PTR_TYPEDEF(IDCompositionTranslateTransform3D);
//_WRL_PTR_TYPEDEF(IDCompositionVirtualSurface);
//_WRL_PTR_TYPEDEF(IDCompositionVisual);

ref class DirectXBase;
delegate void D3DDeviceEventHandler();

// Helper class that initializes DirectX APIs for both 2D and 3D rendering.
// Some of the code in this class may be omitted if only 2D or only 3D rendering is being used.
ref class DirectXBase abstract
{
internal:
    DirectXBase();

    virtual void Initialize(
        Windows::UI::Core::CoreWindow^ window,
        Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ swapChainPanel,
        float dpi
        );
    virtual void HandleDeviceLost();
    virtual void CreateDeviceIndependentResources();
    virtual void CreateDeviceResources();
    virtual void SetDpi(float dpi);
    virtual void UpdateForWindowSizeChange();
    virtual void CreateWindowSizeDependentResources();
    virtual void Render() = 0;
    virtual void Present();
    virtual float ConvertDipsToPixels(float dips);

    event D3DDeviceEventHandler^ DeviceLost;
    event D3DDeviceEventHandler^ DeviceReset;

protected private:
    void OnStereoEnabledChanged(
        _In_ Platform::Object^ sender
        );
    void CheckStereoEnabledStatus();

    Platform::Agile<Windows::UI::Core::CoreWindow>  m_window;
    Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ m_swapChainPanel;

    // DirectWrite & Windows Imaging Component Objects.
    Microsoft::WRL::ComPtr<IDWriteFactory1>         m_dwriteFactory;
    Microsoft::WRL::ComPtr<IWICImagingFactory2>     m_wicFactory;

    // DirectX Core Objects. Required for 2D and 3D.
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_d3dRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_d3dRenderTargetViewRight;

    // Direct2D Rendering Objects. Required for 2D.
    Microsoft::WRL::ComPtr<ID2D1Factory1>           m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>             m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>      m_d2dContext;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1>            m_d2dTargetBitmap;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1>            m_d2dTargetBitmapRight;

    // Direct3D Rendering Objects. Required for 3D.
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_d3dDepthStencilView;

    // Cached renderer properties.
    D3D_FEATURE_LEVEL                               m_featureLevel;
    Windows::Foundation::Size                       m_renderTargetSize;
    Windows::Foundation::Rect                       m_windowBounds;
    float                                           m_dpi;
    Windows::Graphics::Display::DisplayOrientations m_orientation;
    bool                                            m_stereoEnabled;

    // Transforms used for display orientation.
    D2D1::Matrix3x2F                                m_rotationTransform2D;
    DirectX::XMFLOAT4X4                             m_rotationTransform3D;
};
