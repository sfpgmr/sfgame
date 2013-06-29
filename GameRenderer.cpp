//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "ConstantBuffers.h"
#include "DirectXSample.h"
#include "GameRenderer.h"
#include "sfmf.h"
#include "BasicLoader.h"
#include "windows.ui.xaml.media.dxinterop.h"
#include "game.h"

using namespace concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace sf;

//----------------------------------------------------------------------

GameRenderer::GameRenderer() :
  m_initialized(false),
  m_gameResourcesLoaded(false),
  m_levelResourcesLoaded(false),
  m_d2dScale(5.0f),
  m_d3dScale(0.01f),
  m_videoViewport(0.0f,0.0f,(float)VIDEO_WIDTH,(float)VIDEO_HEIGHT),
  m_captureVideo(false),
  m_drawd2d(false)
{

  m_clearColor[0] = 0.0f;
  m_clearColor[1] = 0.0f;
  m_clearColor[2] = 0.0f;
  m_clearColor[3] = 1.0f;
  m_mf.reset(new sf::AutoMF);
}

//----------------------------------------------------------------------

void GameRenderer::Initialize(
  _In_ CoreWindow^ window,
  _In_ SwapChainBackgroundPanel^ swapChainPanel,
  float dpi
  )
{
  m_gameHud = ref new GameHud();

  DirectXBase::Initialize(window, swapChainPanel, dpi);
}

//----------------------------------------------------------------------

void GameRenderer::HandleDeviceLost()
{
  // On device lost all the device resources are invalid.
  // Set the state of the renderer to not have a pointer to the
  // GameMain object.  It will be reset as a part of the
  // game devices resources being recreated.
  m_game = nullptr;

  DirectXBase::HandleDeviceLost();
}

//----------------------------------------------------------------------

void GameRenderer::CreateDeviceIndependentResources()
{
  DirectXBase::CreateDeviceIndependentResources();
  m_gameHud->CreateDeviceIndependentResources(m_dwriteFactory.Get(), m_wicFactory.Get());
}

//----------------------------------------------------------------------

void GameRenderer::CreateDeviceResources()
{
  m_gameResourcesLoaded = false;
  m_levelResourcesLoaded = false;

  DirectXBase::CreateDeviceResources();

  m_gameHud->CreateDeviceResources(m_d2dContext.Get());
}

//----------------------------------------------------------------------

void GameRenderer::UpdateForWindowSizeChange()
{
  DirectXBase::UpdateForWindowSizeChange();

  m_gameHud->UpdateForWindowSizeChange(m_windowBounds);

  if (m_game != nullptr)
  {
    // Update the Projection Matrix and the associated Constant Buffer.
    m_game->GameCamera()->SetProjParams(
      XM_PI / 2, m_renderTargetSize.Width / m_renderTargetSize.Height,
      0.01f,
      100.0f
      );
    ConstantBufferChangeOnResize changesOnResize;
    XMStoreFloat4x4(
      &changesOnResize.projection,
      XMMatrixMultiply(
      XMMatrixTranspose(m_game->GameCamera()->Projection()),
      XMMatrixTranspose(XMLoadFloat4x4(&m_rotationTransform3D))
      )
      );

    m_d3dContext->UpdateSubresource(
      m_constantBufferChangeOnResize.Get(),
      0,
      nullptr,
      &changesOnResize,
      0,
      0
      );
    m_game->game().screen_aabb(1.0f /  (m_renderTargetSize.Height / m_renderTargetSize.Width),1.0f,m_d3dScale);
  }
}

//----------------------------------------------------------------------

task<void> GameRenderer::CreateGameDeviceResourcesAsync(_In_ GameMain^ game)
{
  // Create the device dependent game resources.
  // Only the m_d3dDevice is used in this method.  It is expected
  // to not run on the same thread as the GameRenderer was created.
  // Create methods on the m_d3dDevice are free-threaded and are safe while any methods
  // in the m_d3dContext should only be used on a single thread and handled
  // in the FinalizeCreateGameDeviceResources method.
  m_game = game;

  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));

  // Create the constant buffers.
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;
  bd.ByteWidth = (sizeof(ConstantBufferNeverChanges) + 15) / 16 * 16;
  ThrowIfFailed(
    m_d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferNeverChanges)
    );

  bd.ByteWidth = (sizeof(ConstantBufferChangeOnResize) + 15) / 16 * 16;
  ThrowIfFailed(
    m_d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferChangeOnResize)
    );

  bd.ByteWidth = (sizeof(ConstantBufferChangesEveryFrame) + 15) / 16 * 16;
  ThrowIfFailed(
    m_d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferChangesEveryFrame)
    );

  bd.ByteWidth = (sizeof(ConstantBufferChangesEveryPrim) + 15) / 16 * 16;
  ThrowIfFailed(
    m_d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferChangesEveryPrim)
    );

  bd.ByteWidth = (sizeof(ScreenInfo) + 15) / 16 * 16;
  ThrowIfFailed(
    m_d3dDevice->CreateBuffer(&bd, nullptr, &m_screenInfo)
    );

  bd.ByteWidth = (sizeof(BodyInfo) + 15) / 16 * 16;
  ThrowIfFailed(
    m_d3dDevice->CreateBuffer(&bd, nullptr, &m_BodyInfoBuffer)
    );

  D3D11_SAMPLER_DESC sampDesc;
  ZeroMemory(&sampDesc, sizeof(sampDesc));

  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = FLT_MAX;
  ThrowIfFailed(
    m_d3dDevice->CreateSamplerState(&sampDesc, &m_samplerLinear)
    );

  // ビユーポート情報の設定
  m_viewport = CD3D11_VIEWPORT(
        0.0f,
        0.0f,
        m_renderTargetSize.Width,
        m_renderTargetSize.Height
        );

  {
    D3D11_RASTERIZER_DESC1 RasterizerDesc = {
      D3D11_FILL_SOLID,
      D3D11_CULL_NONE,	//ポリゴンの裏表を無くす
      FALSE,
      0,
      0.0f,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE
    };
    m_d3dDevice->CreateRasterizerState1(&RasterizerDesc,&m_2dRasterizerState);
  }

  //UINT num_vieports = 1;
  //m_d3dContext->RSGetViewports(&num_vieports,&m_viewport);

  // Start the async tasks to load the shaders and textures.
  BasicLoader^ loader = ref new BasicLoader(m_d3dDevice.Get());

  std::vector<task<void>> tasks;

  D3D11_FEATURE_DATA_D3D11_OPTIONS  options = {};
  m_d3dDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS,&options,sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS));


  uint32 numElements = ARRAYSIZE(PNTVertexLayout);
  tasks.push_back(loader->LoadShaderAsync("VertexShader.cso", PNTVertexLayout, numElements, &m_vertexShader, &m_vertexLayout));
  tasks.push_back(loader->LoadShaderAsync("VertexShaderFlat.cso", nullptr, numElements, &m_vertexShaderFlat, nullptr));
  tasks.push_back(loader->LoadShaderAsync("2DVertexShader.cso", Vertex2DLayout, ARRAYSIZE(Vertex2DLayout), &m_2dVertexShader, &m_2dVertexLayout));
  tasks.push_back(loader->LoadShaderAsync("VideoVertexShader.cso", VideoVertex2DLayout, ARRAYSIZE(VideoVertex2DLayout), &m_videoVertexShader, &m_videoVertexLayout));


  tasks.push_back(loader->LoadShaderAsync("PixelShader.cso", &m_pixelShader));
  tasks.push_back(loader->LoadShaderAsync("PixelShaderFlat.cso", &m_pixelShaderFlat));
  tasks.push_back(loader->LoadShaderAsync("2DPixelShader.cso", &m_2dPixelShader));
  tasks.push_back(loader->LoadShaderAsync("VideoPixelShader.cso", &m_videoPixelShader));

  // Make sure the previous versions if any of the textures are released.
  //m_sphereTexture = nullptr;
  //m_cylinderTexture = nullptr;
  //for (uint32 i = 0; i < GameConstants::MaxBackgroundTextures; i++)
  //{
  //    m_ceilingTexture[i] = nullptr;
  //    m_floorTexture[i] = nullptr;
  //    m_wallsTexture[i] = nullptr;
  //}

  // Load Game specific textures.
  //tasks.push_back(loader->LoadTextureAsync("seafloor.dds", nullptr, &m_sphereTexture));
  //tasks.push_back(loader->LoadTextureAsync("metal_texture.dds", nullptr, &m_cylinderTexture));
  //tasks.push_back(loader->LoadTextureAsync("cellceiling.dds", nullptr, &m_ceilingTexture[0]));
  //tasks.push_back(loader->LoadTextureAsync("cellfloor.dds", nullptr, &m_floorTexture[0]));
  //tasks.push_back(loader->LoadTextureAsync("cellwall.dds", nullptr, &m_wallsTexture[0]));
  //tasks.push_back(loader->LoadTextureAsync("nightceiling.dds", nullptr, &m_ceilingTexture[1]));
  //tasks.push_back(loader->LoadTextureAsync("nightfloor.dds", nullptr, &m_floorTexture[1]));
  //tasks.push_back(loader->LoadTextureAsync("nightwall.dds", nullptr, &m_wallsTexture[1]));
  //tasks.push_back(loader->LoadTextureAsync("dayceiling.dds", nullptr, &m_ceilingTexture[2]));
  //tasks.push_back(loader->LoadTextureAsync("dayfloor.dds", nullptr, &m_floorTexture[2]));
  //tasks.push_back(loader->LoadTextureAsync("daywall.dds", nullptr, &m_wallsTexture[2]));

  //
  // ビデオ書き込み用リソースの生成
  //
  if(m_swapChain){
    tasks.push_back(create_task([this]()
    {

      // スワップチェインのバッファを取得する
      ID3D11Texture2DPtr buffer;
      ThrowIfFailed(m_swapChain->GetBuffer(0,IID_PPV_ARGS(&buffer)));
      D3D11_TEXTURE2D_DESC desc;
      buffer->GetDesc(&desc);

      // スワップチェインバッファの中身の複製を保存するテクスチャの作成
      desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
      desc.MiscFlags = 0;
      desc.CPUAccessFlags = 0;
      desc.Usage = D3D11_USAGE_DEFAULT;

      ThrowIfFailed(m_d3dDevice->CreateTexture2D(&desc,nullptr,&m_videoSrcTexure));
      // 複製テクスチャー用のSRV
      CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(
        m_videoSrcTexure.Get(),
        D3D11_SRV_DIMENSION_TEXTURE2D
        );
      ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(m_videoSrcTexure.Get(),&shaderResourceViewDesc,&m_videoSrcView));

      // ビデオサイズに縮小した画像を格納するためのテクスチャーを作成
      desc.Width = VIDEO_WIDTH;
      desc.Height = VIDEO_HEIGHT;
      desc.BindFlags = D3D11_BIND_RENDER_TARGET;

      ThrowIfFailed(m_d3dDevice->CreateTexture2D(&desc,0,&m_videoDestTexture));

      // RTVの作成
      CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(m_videoDestTexture.Get(),
        D3D11_RTV_DIMENSION_TEXTURE2D,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        0,
        0,
        1
        );

      ThrowIfFailed(
        m_d3dDevice->CreateRenderTargetView(
        m_videoDestTexture.Get(),
        &renderTargetViewDesc,
        &m_videoRenderTargetView
        )
        );

      D3D11_RASTERIZER_DESC1 RasterizerDesc = {
        D3D11_FILL_SOLID,
        D3D11_CULL_NONE,	//ポリゴンの裏表を無くす
        FALSE,
        0,
        0.0f,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        FALSE
      };

      // テクスチャ・サンプラの生成
      D3D11_SAMPLER_DESC sampDesc = {};

      sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
      sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
      sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
      sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
      sampDesc.MaxAnisotropy = 1;
      sampDesc.MinLOD = 0;
      sampDesc.MaxLOD = FLT_MAX;
      ThrowIfFailed(
        m_d3dDevice->CreateSamplerState(&sampDesc, &m_videoSamplerState));

      m_d3dDevice->CreateRasterizerState1(&RasterizerDesc,&m_videoRasterState);

      // CPUから読み取り可能なサーフェースを生成
      desc.BindFlags = 0;
      desc.MiscFlags = 0;
      desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
      desc.Usage = D3D11_USAGE_STAGING;

      ThrowIfFailed(m_d3dDevice->CreateTexture2D(&desc,0,&m_videoStageTexture));

      Windows::Storage::StorageFile^ st = create_task(Windows::Storage::KnownFolders::VideosLibrary->CreateFileAsync(L"output.m4v",Windows::Storage::CreationCollisionOption::ReplaceExisting)).get();

      // テクスチャーをビデオストリームに保存するクラスの作成
      m_videoWriter  = ref new sf::VideoWriter(create_task(st->OpenAsync(Windows::Storage::FileAccessMode::ReadWrite)).get());
    }));
  }

  tasks.push_back(create_task([]()
  {
    // Simulate loading additional resources by introducing a delay.
    wait(GameConstants::InitialLoadingDelay);
  }));

    m_game->game().screen_aabb(1.0f / ( m_renderTargetSize.Height / m_renderTargetSize.Width),1.0f,m_d3dScale);

//  m_game->game().screen_aabb(m_windowBounds.Width,m_windowBounds.Height,m_d2dScale);
  // sf::WriteAsync(m_videoStream);
  //m_videoWriter = ref new sf::VideoWriter(m_videoStream);

  // Return the task group of all the async tasks for loading the shader and texture assets.
  return when_all(tasks.begin(), tasks.end());
}

//----------------------------------------------------------------------

void GameRenderer::FinalizeCreateGameDeviceResources()
{
  // All asynchronously loaded resources have completed loading.
  // Now associate all the resources with the appropriate game objects.
  // This method is expected to run in the same thread as the GameRenderer
  // was created. All work will happen behind the "Loading ..." screen after the
  // main loop has been entered.

  // Initialize the Constant buffer with the light positions
  // These are handled here to ensure that the d3dContext is only
  // used in one thread.

  ConstantBufferNeverChanges constantBufferNeverChanges;
  constantBufferNeverChanges.lightPosition[0] = XMFLOAT4( 3.5f, 2.5f,  5.5f, 1.0f);
  constantBufferNeverChanges.lightPosition[1] = XMFLOAT4( 3.5f, 2.5f, -5.5f, 1.0f);
  constantBufferNeverChanges.lightPosition[2] = XMFLOAT4(-3.5f, 2.5f, -5.5f, 1.0f);
  constantBufferNeverChanges.lightPosition[3] = XMFLOAT4( 3.5f, 2.5f,  5.5f, 1.0f);
  constantBufferNeverChanges.lightColor = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
  m_d3dContext->UpdateSubresource(m_constantBufferNeverChanges.Get(), 0, nullptr, &constantBufferNeverChanges, 0, 0);

  ScreenInfo screenInfo = {};
  screenInfo.offset = XMFLOAT2(0.0f,0.0f);
  screenInfo.aspect = XMFLOAT2(m_windowBounds.Height / m_windowBounds.Width,1.0f);
  screenInfo.scale = XMFLOAT2(m_d3dScale,m_d3dScale);
  m_d3dContext->UpdateSubresource(m_screenInfo.Get(), 0, nullptr, &screenInfo, 0, 0);

  // For the targets, there are two unique generated textures.
  // Each texture image includes the number of the texture.
  // Make sure the 2D rendering is occurring on the same thread
  // as the main rendering.
  /*
  TargetTexture^ textureGenerator = ref new TargetTexture(
  m_d3dDevice.Get(),
  m_d2dFactory.Get(),
  m_dwriteFactory.Get(),
  m_d2dContext.Get()
  );

  MeshObject^ cylinderMesh = ref new CylinderMesh(m_d3dDevice.Get(), 26);
  MeshObject^ targetMesh = ref new FaceMesh(m_d3dDevice.Get());
  MeshObject^ sphereMesh = ref new SphereMesh(m_d3dDevice.Get(), 26);

  Material^ cylinderMaterial = ref new Material(
  XMFLOAT4(0.8f, 0.8f, 0.8f, .5f),
  XMFLOAT4(0.8f, 0.8f, 0.8f, .5f),
  XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
  15.0f,
  m_cylinderTexture.Get(),
  m_vertexShader.Get(),
  m_pixelShader.Get()
  );

  Material^ sphereMaterial = ref new Material(
  XMFLOAT4(0.8f, 0.4f, 0.0f, 1.0f),
  XMFLOAT4(0.8f, 0.4f, 0.0f, 1.0f),
  XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
  50.0f,
  m_sphereTexture.Get(),
  m_vertexShader.Get(),
  m_pixelShader.Get()
  );

  auto objects = m_game->RenderObjects();

  // Attach the textures to the appropriate game objects.
  for (auto object = objects.begin(); object != objects.end(); object++)
  {
  if ((*object)->TargetId() == GameConstants::WorldFloorId)
  {
  (*object)->NormalMaterial(
  ref new Material(
  XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
  XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
  XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
  150.0f,
  m_floorTexture[0].Get(),
  m_vertexShaderFlat.Get(),
  m_pixelShaderFlat.Get()
  )
  );
  (*object)->Mesh(ref new WorldFloorMesh(m_d3dDevice.Get()));
  }
  else if ((*object)->TargetId() == GameConstants::WorldCeilingId)
  {
  (*object)->NormalMaterial(
  ref new Material(
  XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
  XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
  XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
  150.0f,
  m_ceilingTexture[0].Get(),
  m_vertexShaderFlat.Get(),
  m_pixelShaderFlat.Get()
  )
  );
  (*object)->Mesh(ref new WorldCeilingMesh(m_d3dDevice.Get()));
  }
  else if ((*object)->TargetId() == GameConstants::WorldWallsId)
  {
  (*object)->NormalMaterial(
  ref new Material(
  XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
  XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f),
  XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
  150.0f,
  m_wallsTexture[0].Get(),
  m_vertexShaderFlat.Get(),
  m_pixelShaderFlat.Get()
  )
  );
  (*object)->Mesh(ref new WorldWallsMesh(m_d3dDevice.Get()));
  }
  else if (Cylinder^ cylinder = dynamic_cast<Cylinder^>(*object))
  {
  cylinder->Mesh(cylinderMesh);
  cylinder->NormalMaterial(cylinderMaterial);
  }
  else if (Face^ target = dynamic_cast<Face^>(*object))
  {
  const int bufferLength = 16;
  char16 str[bufferLength];
  int len = swprintf_s(str, bufferLength, L"%d", target->TargetId());
  Platform::String^ string = ref new Platform::String(str, len);

  ComPtr<ID3D11ShaderResourceView> texture;
  textureGenerator->CreateTextureResourceView(string, &texture);
  target->NormalMaterial(
  ref new Material(
  XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f),
  XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f),
  XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
  5.0f,
  texture.Get(),
  m_vertexShader.Get(),
  m_pixelShader.Get()
  )
  );

  textureGenerator->CreateHitTextureResourceView(string, &texture);
  target->HitMaterial(
  ref new Material(
  XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f),
  XMFLOAT4(0.8f, 0.8f, 0.8f, 0.5f),
  XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
  5.0f,
  texture.Get(),
  m_vertexShader.Get(),
  m_pixelShader.Get()
  )
  );

  target->Mesh(targetMesh);
  }
  else if (Sphere^ sphere = dynamic_cast<Sphere^>(*object))
  {
  sphere->Mesh(sphereMesh);
  sphere->NormalMaterial(sphereMaterial);
  }
  }
  */
  // Ensure that the camera has been initialized with the right Projection
  // matrix.  The camera is not created at the time the first window resize event
  // occurs.
  m_game->GameCamera()->SetProjParams(
  XM_PI / 2,
  m_renderTargetSize.Width / m_renderTargetSize.Height,
  0.01f,
  100.0f
  );

  // Make sure that Projection matrix has been set in the constant buffer
  // now that all the resources are loaded.
  // DirectXBase is handling screen rotations directly to eliminate an unaligned
  // fullscreen copy.  As a result, it is necessary to post multiply the rotationTransform3D
  // matrix to the camera projection matrix.
  // The matrices are transposed due to the shader code expecting the matrices in the opposite
  // row/column order from the DirectX math library.
  ConstantBufferChangeOnResize changesOnResize;
  XMStoreFloat4x4(
  &changesOnResize.projection,
  XMMatrixMultiply(
  XMMatrixTranspose(m_game->GameCamera()->Projection()),
  XMMatrixTranspose(XMLoadFloat4x4(&m_rotationTransform3D))
  )
  );

  m_d3dContext->UpdateSubresource(
  m_constantBufferChangeOnResize.Get(),
  0,
  nullptr,
  &changesOnResize,
  0,
  0
  );

  float adjust_width  = (m_windowBounds.Width / m_windowBounds.Height) / (((float)VIDEO_WIDTH / (float)VIDEO_HEIGHT));
  VertexVideo videoVerticies[4] =
  {
    {DirectX::XMFLOAT2(-adjust_width,-1.0f),DirectX::XMFLOAT2(0.0f,0.0f)},
    {DirectX::XMFLOAT2(-adjust_width,1.0f),DirectX::XMFLOAT2(0.0f,1.0f)},
    {DirectX::XMFLOAT2(adjust_width,-1.0f),DirectX::XMFLOAT2(1.0f,0.0f)},
    {DirectX::XMFLOAT2(adjust_width,1.0f),DirectX::XMFLOAT2(1.0f,1.0f)}
  };


  //頂点バッファ作成
  D3D11_BUFFER_DESC BufferDesc;
  BufferDesc.ByteWidth = sizeof(VertexVideo) * ARRAYSIZE(videoVerticies);
  BufferDesc.Usage = D3D11_USAGE_DEFAULT;
  BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  BufferDesc.CPUAccessFlags = 0;
  BufferDesc.MiscFlags = 0;
  BufferDesc.StructureByteStride = sizeof(float);

  D3D11_SUBRESOURCE_DATA SubResourceData;
  SubResourceData.pSysMem = videoVerticies;
  SubResourceData.SysMemPitch = 0;
  SubResourceData.SysMemSlicePitch = 0;

  ThrowIfFailed(m_d3dDevice->CreateBuffer(&BufferDesc, &SubResourceData, &m_videoVertex));

  //頂点バッファ作成

  for(int i = 0;i < MAX_2D_VERTICES;++i)
  {
    m_2dVertex[i].position = XMFLOAT3(0.0f,0.0f,0.0f);
  }

  BufferDesc.ByteWidth = sizeof(Vertex2D) * MAX_2D_VERTICES;
  BufferDesc.Usage = D3D11_USAGE_DEFAULT;
  BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  BufferDesc.CPUAccessFlags = 0;
  BufferDesc.MiscFlags = 0;
  BufferDesc.StructureByteStride = sizeof(float);

  SubResourceData.pSysMem = m_2dVertex.data();

  ThrowIfFailed(m_d3dDevice->CreateBuffer(&BufferDesc, &SubResourceData, &m_2dVertexBuffer));

  // インデックスバッファ作成
  for(int i = 0;i < MAX_2D_INDEX;++i)
  {
    m_2dIndex[i] = 0;
  }

  BufferDesc.ByteWidth = sizeof(uint32) * MAX_2D_INDEX;
  BufferDesc.Usage = D3D11_USAGE_DEFAULT;
  BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  BufferDesc.CPUAccessFlags = 0;
  BufferDesc.MiscFlags = 0;
  BufferDesc.StructureByteStride = sizeof(uint32);

  SubResourceData.pSysMem = m_2dIndex.data();

  ThrowIfFailed(m_d3dDevice->CreateBuffer(&BufferDesc,&SubResourceData,&m_2dIndexBuffer));

  m_gameResourcesLoaded = true;
}

//----------------------------------------------------------------------

task<void> GameRenderer::LoadLevelResourcesAsync()
{
  m_levelResourcesLoaded = false;

  return create_task([this]()
  {
    // This is where additional async loading of level specific resources
    // would be done.  Since there aren't any to load, just simulate
    // by delaying for some time.
    wait(GameConstants::LevelLoadingDelay);
  });
}

//----------------------------------------------------------------------

void GameRenderer::FinalizeLoadLevelResources()
{
  // Once the level specific resources had been loaded, this method is
  // where D3D context specific actions would be handled.  This method
  // runs in the same thread context as the GameRenderer was created.

  m_levelResourcesLoaded = true;
}

//--------------------------------------------------------------------------------------

void GameRenderer::SetBackground(uint32 background)
{
  //if (background < GameConstants::MaxBackgroundTextures)
  //{
  //    if (m_ceilingTexture[background] != nullptr &&
  //        m_floorTexture[background] != nullptr &&
  //        m_wallsTexture[background] != nullptr)
  //    {
  //        auto objects = m_game->RenderObjects();

  //        // Attach the textures to the appropriate game objects.
  //        for (auto object = objects.begin(); object != objects.end(); object++)
  //        {
  //            if ((*object)->TargetId() == GameConstants::WorldFloorId)
  //            {
  //                (*object)->NormalMaterial()->SetTexture(m_floorTexture[background].Get());
  //            }
  //            else if ((*object)->TargetId() == GameConstants::WorldCeilingId)
  //            {
  //                (*object)->NormalMaterial()->SetTexture(m_ceilingTexture[background].Get());
  //            }
  //            else if ((*object)->TargetId() == GameConstants::WorldWallsId)
  //            {
  //                (*object)->NormalMaterial()->SetTexture(m_wallsTexture[background].Get());
  //            }
  //        }
  //    }
  //}
}

//----------------------------------------------------------------------

void GameRenderer::Render()
{
  int renderingPasses = 1;

  if (m_stereoEnabled)
  {
    renderingPasses = 2;
  }

  for (int i = 0; i < renderingPasses; i++)
  {
    // Iterate through the number of rendering passes to be completed.
    if (m_stereoEnabled && i > 0)
    {
      // Doing the Right Eye View.

      m_d3dContext->RSSetViewports(1,&m_viewport);
      m_d3dContext->OMSetRenderTargets(1, m_d3dRenderTargetViewRight.GetAddressOf(), m_d3dDepthStencilView.Get());
      m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
      m_d2dContext->SetTarget(m_d2dTargetBitmapRight.Get());
    }
    else
    {
      // Doing the Mono or Left Eye View.
      m_d3dContext->RSSetViewports(1,&m_viewport);
      m_d3dContext->OMSetRenderTargets(1, m_d3dRenderTargetView.GetAddressOf(), m_d3dDepthStencilView.Get());
      m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
      m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
    }



    //if (m_game != nullptr && m_gameResourcesLoaded && m_levelResourcesLoaded)
    //{
    //    // This section is only used after the game state has been initialized and all device
    //    // resources needed for the game have been created and associated with the game objects.
    //    if (m_stereoEnabled)
    //    {
    //        // When doing stereo, it is necessary to update the projection matrix once per rendering pass.
    //        ConstantBufferChangeOnResize changesOnResize;
    //        XMStoreFloat4x4(
    //            &changesOnResize.projection,
    //            XMMatrixMultiply(
    //                XMMatrixTranspose(
    //                    i == 0 ?
    //                    m_game->GameCamera()->LeftEyeProjection() :
    //                    m_game->GameCamera()->RightEyeProjection()
    //                    ),
    //                XMMatrixTranspose(XMLoadFloat4x4(&m_rotationTransform3D))
    //                )
    //            );

    //        m_d3dContext->UpdateSubresource(
    //            m_constantBufferChangeOnResize.Get(),
    //            0,
    //            nullptr,
    //            &changesOnResize,
    //            0,
    //            0
    //            );
    //    }
    //    // Update variables that change once per frame.

    //    ConstantBufferChangesEveryFrame constantBufferChangesEveryFrame;
    //    XMStoreFloat4x4(
    //        &constantBufferChangesEveryFrame.view,
    //        XMMatrixTranspose(m_game->GameCamera()->View())
    //        );
    //    m_d3dContext->UpdateSubresource(
    //        m_constantBufferChangesEveryFrame.Get(),
    //        0,
    //        nullptr,
    //        &constantBufferChangesEveryFrame,
    //        0,
    //        0
    //        );

    //    // Setup the graphics pipeline. This sample uses the same InputLayout and set of
    //    // constant buffers for all shaders, so they only need to be set once per frame.

    //    m_d3dContext->IASetInputLayout(m_vertexLayout.Get());
    //    m_d3dContext->VSSetConstantBuffers(0, 1, m_constantBufferNeverChanges.GetAddressOf());
    //    m_d3dContext->VSSetConstantBuffers(1, 1, m_constantBufferChangeOnResize.GetAddressOf());
    //    m_d3dContext->VSSetConstantBuffers(2, 1, m_constantBufferChangesEveryFrame.GetAddressOf());
    //    m_d3dContext->VSSetConstantBuffers(3, 1, m_constantBufferChangesEveryPrim.GetAddressOf());

    //    m_d3dContext->PSSetConstantBuffers(2, 1, m_constantBufferChangesEveryFrame.GetAddressOf());
    //    m_d3dContext->PSSetConstantBuffers(3, 1, m_constantBufferChangesEveryPrim.GetAddressOf());
    //    m_d3dContext->PSSetSamplers(0, 1, m_samplerLinear.GetAddressOf());

    //    auto objects = m_game->RenderObjects();
    //    for (auto object = objects.begin(); object != objects.end(); object++)
    //    {
    //        (*object)->Render(m_d3dContext.Get(), m_constantBufferChangesEveryPrim.Get());
    //    }
    //}
    //else
    {
      const float ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

      // Only need to clear the background when not rendering the full 3D scene since
      // the 3D world is a fully enclosed box and the dynamics prevents the camera from
      // moving outside this space.
      if (m_stereoEnabled && i > 0)
      {
        // Doing the Right Eye View.
        m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetViewRight.Get(), ClearColor);
      }
      else
      {
        // Doing the Mono or Left Eye View.
        m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetView.Get(), ClearColor);
      }
    }

    // Box2Dオブジェクトの描画
    if(!m_drawd2d && m_game != nullptr && m_gameResourcesLoaded)
    {
      RenderBox2DObjectsD3D();
    }

    m_d2dContext->BeginDraw();

    // To handle the swapchain being pre-rotated, set the D2D transformation to include it.
    m_d2dContext->SetTransform(m_rotationTransform2D);



    if (m_game != nullptr && m_gameResourcesLoaded)
    {
      if(m_drawd2d){
        RenderBox2DObjectsD2D();
      }
      // This is only used after the game state has been initialized.
      m_gameHud->Render(m_game, m_d2dContext.Get(), m_windowBounds);
    }

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
      ThrowIfFailed(hr);
    }

    // Video Frameの書き込み

    //if(m_texture[0] && m_texture[1]){
    //static int index = 0;
    //ID3D11Texture2DPtr pSrcBuffer;
    //ThrowIfFailed(m_swapChain->GetBuffer(0,IID_PPV_ARGS(&pSrcBuffer)));
    //m_d3dContext->CopyResource(m_texture[index].Get(),pSrcBuffer.Get());
    //m_d2dContext->SetTarget(m_videoTarget.Get());
    ////m_d2dContext->SetTarget(m_d2dTargetBitmapRight.Get());
    //m_d2dContext->BeginDraw();
    //m_d2dContext->DrawBitmap(m_2dTexture[index].Get());
    //m_d2dContext->EndDraw();
    //++index;
    //index = index & 1;
    //}
  }
  Present();

  if(m_captureVideo){
    WriteVideoFrame();
  }
}

void GameRenderer::ClearScreen()
{
  int renderingPasses = 1;

  if (m_stereoEnabled)
  {
    renderingPasses = 2;
  }
 for (int i = 0; i < renderingPasses; i++)
  {
    // Iterate through the number of rendering passes to be completed.
    if (m_stereoEnabled && i > 0)
    {
      // Doing the Right Eye View.

      m_d3dContext->RSSetViewports(1,&m_viewport);
      m_d3dContext->OMSetRenderTargets(1, m_d3dRenderTargetViewRight.GetAddressOf(), m_d3dDepthStencilView.Get());
      m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
    else
    {
      // Doing the Mono or Left Eye View.
      m_d3dContext->RSSetViewports(1,&m_viewport);
      m_d3dContext->OMSetRenderTargets(1, m_d3dRenderTargetView.GetAddressOf(), m_d3dDepthStencilView.Get());
      m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
 }
   Present();
}
void GameRenderer::RenderBox2DObjectsD2D()
{

  sf::game_base& game(m_game->game());

  b2World& world(game.world());

  // Box2Dオブジェクトの描画
  b2Body* body = world.GetBodyList();
  int32 body_count = world.GetBodyCount();
  //D2D1::Matrix3x2F mat = D2D1::Matrix3x2F::Identity();
  //mat._22 = -1.0f;
  //mat = mat * D2D1::Matrix3x2F::Scale(10.0f,10.0f);
  //D2D1::Matrix3x2F matt = D2D1::Matrix3x2F::Translation(width_ / 2.0f,height_ /2.0f);
  //mat._31 = matt._31;
  //mat._32 = matt._32;

  b2Mat22 mat;
  //       b2Mat33 mat;
  float32 scale = m_d2dScale;
  mat.SetZero();
  mat.ex.x = 1.0f * scale;
  mat.ey.y = -1.0f * scale;
  b2Vec2 screen;
  screen.x = m_renderTargetSize.Width  / 2.0f;
  screen.y = m_renderTargetSize.Height / 2.0f;
  ID2D1SolidColorBrushPtr brush,line_brush,obj_brush,edge_brush,circle_brush;

  m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &brush);
  m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(1.0f,1.0f,1.0f,0.5f), &line_brush);
  m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.2f,0.2f,1.0f,1.0f), &obj_brush);
  m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(1.0f,0.5f,0.5f,1.0f), &edge_brush);
  m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.5f,1.0f,0.5f,1.0f), &circle_brush);

  //        float32 limit_y = -screen.y;
  //mat.ez.x = width_ / 2.0f;
  //mat.ez.y = height_ / 2.0f;

  while(body)
  {
    b2Fixture* fix = body->GetFixtureList();
    b2Transform transform = body->GetTransform();
    transform.p.x -= game.scroll_offset();
    while(fix){
      b2Shape* s = fix->GetShape();
      switch(s->GetType())
      {
      case  b2Shape::e_polygon:
        {
          b2PolygonShape* sp = static_cast<b2PolygonShape*>(s);
          ID2D1PathGeometry1Ptr geometry;
          //        ID2D1RectangleGeometryPtr 
          m_d2dFactory->CreatePathGeometry(&geometry);
          ID2D1GeometrySinkPtr sink;
          geometry->Open(&sink);

          int32 vcount = sp->GetVertexCount();

          b2Vec2 v1 = sp->GetVertex(0);// + vb;
          v1 = b2Mul(transform,v1);
          v1 = b2Mul(mat,v1) + screen;


          sink->BeginFigure(D2D1::Point2F(v1.x ,v1.y),D2D1_FIGURE_BEGIN_FILLED);
          for(int j = 1;j < vcount;++j)
          {
            b2Vec2 v = sp->GetVertex(j);// + vb;
            v = b2Mul(transform,v);
            v = b2Mul(mat,v) + screen;
            sink->AddLine(D2D1::Point2F(v.x,v.y));
          }
          sink->EndFigure(D2D1_FIGURE_END_CLOSED);
          sink->Close();

          m_d2dContext->FillGeometry(geometry.Get(),obj_brush.Get());

        }
        break;
      case  b2Shape::e_circle:
        {
          b2CircleShape* circle = static_cast<b2CircleShape*>(s);
          b2Vec2 center = b2Mul(mat,b2Mul(transform, circle->m_p));
          float32 radius = circle->m_radius * scale;
          b2Vec2 axis = b2Mul(transform.q, b2Vec2(1.0f, 0.0f));
          m_d2dContext->FillEllipse(D2D1::Ellipse(D2D1::Point2F(center.x + screen.x,center.y + screen.y),radius,radius),circle_brush.Get());
        }
        break;
      case b2Shape::e_edge:
        {
          b2EdgeShape* edge = static_cast<b2EdgeShape*>(s);
          b2Vec2 v1 = b2Mul(mat,b2Mul(transform, edge->m_vertex1)) + screen;
          b2Vec2 v2 = b2Mul(mat,b2Mul(transform, edge->m_vertex2)) + screen;
          m_d2dContext->DrawLine(D2D1::Point2F(v1.x ,v1.y),D2D1::Point2F(v2.x,v2.y),obj_brush.Get());

        }
        break;

      case b2Shape::e_chain:
        {
          ID2D1PathGeometry1Ptr geometry;
          //        ID2D1RectangleGeometryPtr 
          m_d2dFactory->CreatePathGeometry(&geometry);
          ID2D1GeometrySinkPtr sink;
          geometry->Open(&sink);

          b2ChainShape* chain = static_cast<b2ChainShape*>(s);
          int32 count = chain->m_count;
          const b2Vec2* vertices = chain->m_vertices;


          b2Vec2 v1 = b2Mul(mat,b2Mul(transform, vertices[0])) + screen;
          sink->BeginFigure(D2D1::Point2F(v1.x ,v1.y),D2D1_FIGURE_BEGIN_FILLED);
          for (int32 i = 1; i < count; ++i)
          {
            b2Vec2 v2 = b2Mul(mat,b2Mul(transform, vertices[i])) + screen;
            sink->AddLine(D2D1::Point2F(v2.x,v2.y));
          }
          sink->EndFigure(D2D1_FIGURE_END_CLOSED);
          sink->Close();
          m_d2dContext->FillGeometry(geometry.Get(),edge_brush.Get());
        }
        break;
      }
      fix = fix->GetNext();
    }
    body = body->GetNext();
  }
}

void GameRenderer::RenderBox2DObjectsD3D()
{

  sf::game_base& game(m_game->game());

  b2World& world(game.world());

  m_d3dContext->IASetInputLayout(m_2dVertexLayout.Get());
  uint32 stride = sizeof(Vertex2D);
  uint32 offset = 0;
  m_d3dContext->IASetVertexBuffers(0,1,m_2dVertexBuffer.GetAddressOf(),&stride,&offset);
  m_d3dContext->IASetIndexBuffer(m_2dIndexBuffer.Get(),DXGI_FORMAT_R32_UINT,0);
  m_d3dContext->VSSetConstantBuffers(0, 1, m_screenInfo.GetAddressOf());
  m_d3dContext->VSSetConstantBuffers(1, 1, m_BodyInfoBuffer.GetAddressOf());
  m_d3dContext->VSSetShader(m_2dVertexShader.Get(),nullptr,0);

  m_d3dContext->PSSetShader(m_2dPixelShader.Get(),nullptr,0);
  m_d3dContext->PSSetSamplers(0,0,nullptr);

  m_d3dContext->RSSetState(m_2dRasterizerState.Get());

  //m_d3dContext->PSSetSamplers(0, 1, m_samplerLinear.GetAddressOf());

  // Box2Dオブジェクトの描画
  b2Body* body = world.GetBodyList();
  int32 body_count = world.GetBodyCount();

  while(body)
  {

    b2Fixture* fix = body->GetFixtureList();
    b2Transform transform = body->GetTransform();
    
    m_BodyInfo.position.x = transform.p.x - game.scroll_offset();
    m_BodyInfo.position.y = transform.p.y;
    m_BodyInfo.rotation.x = transform.q.c;
    m_BodyInfo.rotation.y = transform.q.s;
    m_BodyInfo.color = XMFLOAT4(1.0f,0.5f,1.0f,1.0f);

      m_d3dContext->UpdateSubresource(m_BodyInfoBuffer.Get(),0,nullptr,&m_BodyInfo,0,0);

    while(fix){
      b2Shape* s = fix->GetShape();

      switch(s->GetType())
      {
      case  b2Shape::e_polygon:
        {
          b2PolygonShape* sp = static_cast<b2PolygonShape*>(s);

          const int32 vcount = sp->GetVertexCount();
          const int32 triangleNum = vcount - 2;
          int32 vert1 = 0;
          int32 vert2 = 1;
          int32 vert3 = 2;


          // 頂点データのセット
          for(int j = 0;j < vcount;++j)
          {
            const b2Vec2& v = sp->GetVertex(j);// + vb;
            m_2dVertex[j].position = XMFLOAT3(v.x,v.y,0.5f);
          }
          // 頂点バッファを更新する
          m_d3dContext->UpdateSubresource(m_2dVertexBuffer.Get(),0,nullptr,m_2dVertex.data(),0,0);

          // インデックスデータセット
          // 凸多角形を三角形に分割する
          for(int j = 0;j < triangleNum;++j)
          {
            m_2dIndex[j * 3] = 0;
            m_2dIndex[j * 3 + 1] = vert2;
            m_2dIndex[j * 3 + 2] = vert3;
            ++vert2;
            ++vert3;
          }
          // インデックスバッファの更新
          m_d3dContext->UpdateSubresource(m_2dIndexBuffer.Get(),0,nullptr,m_2dIndex.data(),0,0);

          m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
          m_d3dContext->DrawIndexed(triangleNum * 3,0,0);
        }
        break;
      // 円の描画（64角形で近似）
      case  b2Shape::e_circle:
        {
          const int vertices = 64;
          b2CircleShape* circle = static_cast<b2CircleShape*>(s);
          float32 radius = circle->m_radius;
          float radDelta  = 2.0f * XM_PI * (float)1.0f / 64.0f ;
          float rad = 0.0f;

          for(int j = 0,end = vertices / 4;j < end;++j)
          {
            float s = XMScalarSin(rad) * radius;
            float c = XMScalarCos(rad) * radius;
            m_2dVertex[j].position = XMFLOAT3(c + circle->m_p.x,s + circle->m_p.y,0.5f);
            m_2dVertex[31 - j].position = XMFLOAT3(-c + circle->m_p.x,s + circle->m_p.y,0.5f);
            m_2dVertex[j + 32].position = XMFLOAT3(-c + circle->m_p.x,-s + circle->m_p.y,0.5f);
            m_2dVertex[63 - j].position = XMFLOAT3(c  + circle->m_p.x,-s + circle->m_p.y,0.5f);
            rad += radDelta;
          }

          // 頂点バッファを更新する
          m_d3dContext->UpdateSubresource(m_2dVertexBuffer.Get(),0,nullptr,m_2dVertex.data(),0,0);

          const int32 triangleNum = vertices - 2;
          int32 vert1 = 0;
          int32 vert2 = 1;
          int32 vert3 = 2;
          int index_base = 0;

          // インデックスデータセット
          for(int j = 0;j < triangleNum;++j)
          {
            m_2dIndex[index_base] = 0;
            m_2dIndex[index_base + 1] = vert2;
            m_2dIndex[index_base + 2] = vert3;
            index_base += 3;
            ++vert2;
            ++vert3;
          }
          // インデックスバッファの更新
          m_d3dContext->UpdateSubresource(m_2dIndexBuffer.Get(),0,nullptr,m_2dIndex.data(),0,0);

          m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
          m_d3dContext->DrawIndexed(triangleNum * 3,0,0);

          //b2Vec2 axis = b2Mul(transform.q, b2Vec2(1.0f, 0.0f));
          //m_d2dContext->FillEllipse(D2D1::Ellipse(D2D1::Point2F(center.x + screen.x,center.y + screen.y),radius,radius),circle_brush.Get());
        }
        break;
      // エッジの描画
      case b2Shape::e_edge:
        {
          b2EdgeShape* edge = static_cast<b2EdgeShape*>(s);

          const b2Vec2& v1 = edge->m_vertex1;
          const b2Vec2& v2 = edge->m_vertex2;
 
          m_2dVertex[0].position = XMFLOAT3(v1.x,v1.y,0.5f);
          m_2dVertex[1].position = XMFLOAT3(v2.x,v2.y,0.5f);
          
          // 頂点バッファを更新する
          m_d3dContext->UpdateSubresource(m_2dVertexBuffer.Get(),0,nullptr,m_2dVertex.data(),0,0);
          m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
          m_d3dContext->Draw(2,0);
        }
        break;

      case b2Shape::e_chain:
        {
          m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
          b2ChainShape* chain = static_cast<b2ChainShape*>(s);

          int32 count = chain->m_count;
          const b2Vec2* vertices = chain->m_vertices;
          int32 i = 0;
          while(true)
          {
            int end = min(count,MAX_2D_VERTICES);
            int draw_count = 0;
            while (draw_count < end)
            {
              m_2dVertex[draw_count].position = XMFLOAT3(vertices[i].x,vertices[i].y,0.5f);
              ++i;
              ++draw_count;
            }
            // 頂点バッファを更新する
            m_d3dContext->UpdateSubresource(m_2dVertexBuffer.Get(),0,nullptr,m_2dVertex.data(),0,0);
            m_d3dContext->Draw(draw_count,0);
            count -= draw_count;
            if(count <= 0) break;
            ++count;
            --i;
            --i;
          }
        }
        break;
      }
      fix = fix->GetNext();
    }
    body = body->GetNext();
  }
}

void GameRenderer::RenderPause()
{
  int renderingPasses = 1;

  if (m_stereoEnabled)
  {
    renderingPasses = 2;
  }

  for (int i = 0; i < renderingPasses; i++)
  {
    // Iterate through the number of rendering passes to be completed.
    if (m_stereoEnabled && i > 0)
    {
      // Doing the Right Eye View.

      m_d3dContext->RSSetViewports(1,&m_viewport);
      m_d3dContext->OMSetRenderTargets(1, m_d3dRenderTargetViewRight.GetAddressOf(), m_d3dDepthStencilView.Get());
      m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
      m_d2dContext->SetTarget(m_d2dTargetBitmapRight.Get());
    }
    else
    {
      // Doing the Mono or Left Eye View.
      m_d3dContext->RSSetViewports(1,&m_viewport);
      m_d3dContext->OMSetRenderTargets(1, m_d3dRenderTargetView.GetAddressOf(), m_d3dDepthStencilView.Get());
      m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
      m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
    }

    {
      const float ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

      // Only need to clear the background when not rendering the full 3D scene since
      // the 3D world is a fully enclosed box and the dynamics prevents the camera from
      // moving outside this space.
      if (m_stereoEnabled && i > 0)
      {
        // Doing the Right Eye View.
        m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetViewRight.Get(), ClearColor);
      }
      else
      {
        // Doing the Mono or Left Eye View.
        m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetView.Get(), ClearColor);
      }
    }

    m_d2dContext->BeginDraw();

    // To handle the swapchain being pre-rotated, set the D2D transformation to include it.
    m_d2dContext->SetTransform(m_rotationTransform2D);

    if (m_game != nullptr && m_gameResourcesLoaded)
    {
      m_gameHud->RenderPause(m_game,m_d2dContext.Get(),m_windowBounds);
    }

    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
      ThrowIfFailed(hr);
    }

  }

  Present();
  if(m_captureVideo){
    WriteVideoFrame();
  }

}

void GameRenderer::RenderGameOver()
{
  int renderingPasses = 1;

  if (m_stereoEnabled)
  {
    renderingPasses = 2;
  }

  for (int i = 0; i < renderingPasses; i++)
  {
    // Iterate through the number of rendering passes to be completed.
    if (m_stereoEnabled && i > 0)
    {
      // Doing the Right Eye View.

      m_d3dContext->RSSetViewports(1,&m_viewport);
      m_d3dContext->OMSetRenderTargets(1, m_d3dRenderTargetViewRight.GetAddressOf(), m_d3dDepthStencilView.Get());
      m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
      m_d2dContext->SetTarget(m_d2dTargetBitmapRight.Get());
    }
    else
    {
      // Doing the Mono or Left Eye View.
      m_d3dContext->RSSetViewports(1,&m_viewport);
      m_d3dContext->OMSetRenderTargets(1, m_d3dRenderTargetView.GetAddressOf(), m_d3dDepthStencilView.Get());
      m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
      m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
    }

    {
      const float ClearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};

      // Only need to clear the background when not rendering the full 3D scene since
      // the 3D world is a fully enclosed box and the dynamics prevents the camera from
      // moving outside this space.
      if (m_stereoEnabled && i > 0)
      {
        // Doing the Right Eye View.
        m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetViewRight.Get(), ClearColor);
      }
      else
      {
        // Doing the Mono or Left Eye View.
        m_d3dContext->ClearRenderTargetView(m_d3dRenderTargetView.Get(), ClearColor);
      }
    }

    m_d2dContext->BeginDraw();

    // To handle the swapchain being pre-rotated, set the D2D transformation to include it.
    m_d2dContext->SetTransform(m_rotationTransform2D);

    if (m_game != nullptr && m_gameResourcesLoaded)
    {
      m_gameHud->DrawGameOver(m_game,m_d2dContext.Get(),m_windowBounds);
    }

    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
      ThrowIfFailed(hr);
    }

  }

  Present();
  if(m_captureVideo){
    WriteVideoFrame();
  }

}


void GameRenderer::WriteVideoFrame()
{
  if(m_videoWriter && m_gameResourcesLoaded)
  {
    static int count = 0;
    if(!count){
      {
        ID3D11Texture2DPtr pSrcBuffer;
        ThrowIfFailed(m_swapChain->GetBuffer(0,IID_PPV_ARGS(&pSrcBuffer)));
        m_d3dContext->CopyResource(m_videoSrcTexure.Get(),pSrcBuffer.Get());
      }
      uint32 stride = sizeof(VertexVideo);
      uint32 offset = 0;
      m_d3dContext->IASetVertexBuffers(0,1,m_videoVertex.GetAddressOf(),&stride,&offset);
      m_d3dContext->IASetInputLayout(m_videoVertexLayout.Get());
      m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
      m_d3dContext->VSSetShader(m_videoVertexShader.Get(),nullptr,0);
      m_d3dContext->PSSetShader(m_videoPixelShader.Get(),nullptr,0);
      m_d3dContext->OMSetRenderTargets(1,m_videoRenderTargetView.GetAddressOf(),nullptr);
      m_d3dContext->PSSetSamplers(0,1,m_videoSamplerState.GetAddressOf());
      m_d3dContext->PSSetShaderResources(0,1,m_videoSrcView.GetAddressOf());
      m_d3dContext->RSSetState(m_videoRasterState.Get());
      m_d3dContext->RSSetViewports(1,&m_videoViewport);
      m_d3dContext->ClearRenderTargetView(m_videoRenderTargetView.Get(),m_clearColor);
      m_d3dContext->Draw(4,0);
      m_d3dContext->CopyResource(m_videoStageTexture.Get(),m_videoDestTexture.Get());
      m_videoWriter->WriteTextureToBuffer(m_d3dContext,m_videoStageTexture);
    } else {
      m_videoWriter->WriteSink();
    }
    ++count;
    count = count & 1;
  }
}

//----------------------------------------------------------------------

GameHud^ GameRenderer::Hud()
{
  return m_gameHud;
}



#if defined(_DEBUG)
void GameRenderer::ReportLiveDeviceObjects()
{
  // If the debug layer isn't present on the system then this QI will fail.  In that case
  // the routine just exits.   This is a debugging aid to see the active D3D objects and
  // is not critical to functional operation of the sample.

  ComPtr<ID3D11Debug> debugLayer;
  HRESULT hr = m_d3dDevice.As(&debugLayer);
  if (FAILED(hr))
  {
    return;
  }
  ThrowIfFailed(
    debugLayer->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL)
    );
}
#endif

