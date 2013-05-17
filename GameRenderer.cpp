﻿//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "DirectXSample.h"
#include "GameRenderer.h"
#include "ConstantBuffers.h"
#include "BasicLoader.h"
#include "windows.ui.xaml.media.dxinterop.h"
#include "game.h"

using namespace concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;

//----------------------------------------------------------------------

GameRenderer::GameRenderer() :
	m_initialized(false),
	m_gameResourcesLoaded(false),
	m_levelResourcesLoaded(false)
{
}

//----------------------------------------------------------------------

void GameRenderer::Initialize(
	_In_ CoreWindow^ window,
	_In_ SwapChainBackgroundPanel^ swapChainPanel,
	float dpi
	)
{
	m_gameHud = ref new GameHud(
		L"Windows 8 サンプル",
		L"DirectX/XAML とBox2D でゲームを作ってみる。",
		L"License state unknown"
		);

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
	DX::ThrowIfFailed(
		m_d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferNeverChanges)
		);

	bd.ByteWidth = (sizeof(ConstantBufferChangeOnResize) + 15) / 16 * 16;
	DX::ThrowIfFailed(
		m_d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferChangeOnResize)
		);

	bd.ByteWidth = (sizeof(ConstantBufferChangesEveryFrame) + 15) / 16 * 16;
	DX::ThrowIfFailed(
		m_d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferChangesEveryFrame)
		);

	bd.ByteWidth = (sizeof(ConstantBufferChangesEveryPrim) + 15) / 16 * 16;
	DX::ThrowIfFailed(
		m_d3dDevice->CreateBuffer(&bd, nullptr, &m_constantBufferChangesEveryPrim)
		);

	bd.ByteWidth = (sizeof(Buffer2D) + 15) / 16 * 16;
	DX::ThrowIfFailed(
		m_d3dDevice->CreateBuffer(&bd, nullptr, &m_Buffer2D)
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
	DX::ThrowIfFailed(
		m_d3dDevice->CreateSamplerState(&sampDesc, &m_samplerLinear)
		);

	// Start the async tasks to load the shaders and textures.
	BasicLoader^ loader = ref new BasicLoader(m_d3dDevice.Get());

	std::vector<task<void>> tasks;

	uint32 numElements = ARRAYSIZE(PNTVertexLayout);
	tasks.push_back(loader->LoadShaderAsync("VertexShader.cso", PNTVertexLayout, numElements, &m_vertexShader, &m_vertexLayout));
	tasks.push_back(loader->LoadShaderAsync("VertexShaderFlat.cso", nullptr, numElements, &m_vertexShaderFlat, nullptr));
	tasks.push_back(loader->LoadShaderAsync("2DVertexShader.cso", Vertex2DLayout, ARRAYSIZE(Vertex2DLayout), &m_2dVertexShader, &m_2dVertexLayout));
	tasks.push_back(loader->LoadShaderAsync("PixelShader.cso", &m_pixelShader));
	tasks.push_back(loader->LoadShaderAsync("PixelShaderFlat.cso", &m_pixelShaderFlat));
	tasks.push_back(loader->LoadShaderAsync("2DPixelShader.cso", &m_2dPixelShader));

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
	tasks.push_back(create_task([]()
	{
		// Simulate loading additional resources by introducing a delay.
		wait(GameConstants::InitialLoadingDelay);
	}));

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

	Buffer2D buffer2D;
	buffer2D.color = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	XMMATRIX mat = XMMatrixIdentity();
	mat *= XMMatrixScaling(5.0f,5.0f,1.0f);

	XMStoreFloat4x4( &buffer2D.transform,XMMatrixAffineTransformation2D
		(
		XMVectorSet(5.0f,5.0f,1.0f,1.0f),
		XMVectorZero(),
		0.0f,
		XMVectorSet(m_renderTargetSize.Width,m_renderTargetSize.Height,0.0f,0.0f)));

	buffer2D.transform._22 *= -1.0f;
	m_d3dContext->UpdateSubresource(m_Buffer2D.Get(), 0, nullptr, &buffer2D, 0, 0);

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
	*/
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
			m_d3dContext->OMSetRenderTargets(1, m_d3dRenderTargetViewRight.GetAddressOf(), m_d3dDepthStencilView.Get());
			m_d3dContext->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			m_d2dContext->SetTarget(m_d2dTargetBitmapRight.Get());
		}
		else
		{
			// Doing the Mono or Left Eye View.
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

		m_d2dContext->BeginDraw();

		// To handle the swapchain being pre-rotated, set the D2D transformation to include it.
		m_d2dContext->SetTransform(m_rotationTransform2D);



		if (m_game != nullptr && m_gameResourcesLoaded)
		{
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
				float32 scale = 5.0f;
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

			// This is only used after the game state has been initialized.
			m_gameHud->Render(m_game, m_d2dContext.Get(), m_windowBounds);
		}

		// We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
		// is lost. It will be handled during the next call to Present.
		HRESULT hr = m_d2dContext->EndDraw();
		if (hr != D2DERR_RECREATE_TARGET)
		{
			DX::ThrowIfFailed(hr);
		}
	}
	Present();
}

//----------------------------------------------------------------------

GameHud^ GameRenderer::Hud()
{
	return m_gameHud;
}

//----------------------------------------------------------------------

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
	DX::ThrowIfFailed(
		debugLayer->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL)
		);
}

//----------------------------------------------------------------------

#endif

