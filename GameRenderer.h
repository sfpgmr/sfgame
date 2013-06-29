//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// GameRenderer:
// This is the renderer for the game.  It derives from DirectXBase.
// It is responsble for creating and maintaining all the D3D11 and D2D objects
// used to generate the game visuals.  It maintains a reference to the GameMain
// object to retrieve the list of objects to render as well as status of the 
// game for the Heads Up Display (HUD).
//
// The renderer is designed to use a standard vertex layout across all objects.  This 
// simplifies the shader design and allows for easy changes between shaders independent
// of the objects' geometry.  Each vertex is defined by a position, a normal and one set of
// 2D texture coordinates.  The shaders all expect one 2D texture and 4 constant buffers:
//     m_constantBufferNeverChanges - general parameters that are set only once.  This includes
//         all the lights used in scene generation.
//     m_constantBufferChangeOnResize - the projection matrix.  It is typically only changed when
//         the window is resized, however it will be changed 2x per frame when rendering
//         in 3D stereo.
//     m_constantBufferChangesEveryFrame - the view transformation matrix.  This is set once per frame.
//     m_constantBufferChangesEveryPrim - the parameters for each object.  It includes the object to world
//         transformation matrix as well as material properties like color and specular exponent for lighting
//         calculations.
//
// The renderer also maintains a set of texture resources that will be associated with particular game objects.
// It knows which textures are to be associated with which objects and will do that association once the 
// textures have been loaded.
//
// The renderer provides a set of methods to allow for a "standard" sequence to be executed for loading general
// game resources and for level specific resources.  Because D3D11 allows free threaded creation of objects,
// textures will be loaded asynchronously and in parallel, however D3D11 does not allow for multiple threads to
// be using the DeviceContext at the same time.
//
// The pattern is:
//     create_task([this]()
//     {
//         // create/load device resources
//     }).then([this]()
//     {
//         // create/load device context resources
//     }, task_continuation_context::use_current());

#include "DirectXBase.h"
#include "ConstantBuffers.h"
#include "GameHud.h"
#include "gamemain.h"
namespace sf {
ref class GameMain;
ref class GameHud;
ref class VideoWriter;
class AutoMF;
ref class GameRenderer : public DirectXBase
{
internal:
  
  static const int32 MAX_2D_INDEX = 512;
  static const int32 MAX_2D_VERTICES = 64;

  GameRenderer();

  virtual void Initialize(
    _In_ Windows::UI::Core::CoreWindow^ window,
    _In_ Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ swapChainPanel,
    float dpi
    ) override;

  virtual void CreateDeviceIndependentResources() override;
  virtual void CreateDeviceResources() override;
  virtual void UpdateForWindowSizeChange() override;
  virtual void Render() override;
  virtual void ClearScreen();
  virtual void HandleDeviceLost() override;
  void RenderPause();
  void RenderGameOver();
  concurrency::task<void> CreateGameDeviceResourcesAsync(_In_ GameMain^ game);
  void FinalizeCreateGameDeviceResources();
  concurrency::task<void> LoadLevelResourcesAsync();
  void FinalizeLoadLevelResources();

  void SetBackground(uint32 background);

  GameHud^ Hud();

#if defined(_DEBUG)
  void ReportLiveDeviceObjects();
#endif

protected private:

  bool                                                m_initialized;
  bool                                                m_gameResourcesLoaded;
  bool                                                m_levelResourcesLoaded;
  bool                                                m_captureVideo;
  GameHud^                                            m_gameHud;
  GameMain^											                      m_game;
  float                                               m_d2dScale;
  float                                               m_d3dScale;

  //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_sphereTexture;
  //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_cylinderTexture;
  //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_ceilingTexture[GameConstants::MaxBackgroundTextures];
  //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_floorTexture[GameConstants::MaxBackgroundTextures];
  //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_wallsTexture[GameConstants::MaxBackgroundTextures];
  CD3D11_VIEWPORT m_viewport;

  // Constant Buffers
  Microsoft::WRL::ComPtr<ID3D11Buffer>                m_constantBufferNeverChanges;
  Microsoft::WRL::ComPtr<ID3D11Buffer>                m_constantBufferChangeOnResize;
  Microsoft::WRL::ComPtr<ID3D11Buffer>                m_constantBufferChangesEveryFrame;
  Microsoft::WRL::ComPtr<ID3D11Buffer>                m_constantBufferChangesEveryPrim;

  Microsoft::WRL::ComPtr<ID3D11SamplerState>          m_samplerLinear;
  Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vertexShader;
  Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vertexShaderFlat;
  Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShaderFlat;
  Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_vertexLayout;

  // Box2D描画用D3Dリソース

  ID3D11BufferPtr         m_screenInfo;
  BodyInfo                m_BodyInfo;
  ID3D11BufferPtr         m_BodyInfoBuffer;
  ID3D11VertexShaderPtr   m_2dVertexShader;
  ID3D11PixelShaderPtr    m_2dPixelShader;
  ID3D11InputLayoutPtr    m_2dVertexLayout;
  std::array<Vertex2D,MAX_2D_VERTICES>   m_2dVertex;
  ID3D11BufferPtr         m_2dVertexBuffer;
  std::array<uint32, MAX_2D_INDEX>       m_2dIndex;
  ID3D11BufferPtr         m_2dIndexBuffer;
  ID3D11RasterizerState1Ptr  m_2dRasterizerState;


  // スクリーンキャプチャー用リソース

  sf::VideoWriter^ m_videoWriter;
  Windows::Storage::Streams::IRandomAccessStream^ m_videoStream;
  ID3D11Texture2DPtr m_videoSrcTexure;
  ID3D11ShaderResourceViewPtr    m_videoSrcView;
  ID3D11RenderTargetViewPtr m_videoRenderTargetView;
  ID3D11Texture2DPtr m_videoDestTexture;
  ID3D11Texture2DPtr m_videoStageTexture;
  Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_videoVertexShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_videoPixelShader;
  Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_videoVertexLayout;
  ID3D11BufferPtr                 m_videoVertex;
  ID3D11RasterizerState1Ptr  m_videoRasterState;
  ID3D11SamplerStatePtr m_videoSamplerState;
  float m_clearColor[4];
  CD3D11_VIEWPORT m_videoViewport; 
  std::unique_ptr<sf::AutoMF> m_mf;
  bool m_drawd2d;

private:
  void RenderBox2DObjectsD2D();
  void RenderBox2DObjectsD3D();
  void WriteVideoFrame();

};
}
