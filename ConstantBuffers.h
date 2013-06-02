//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

 struct VertexVideo {
		DirectX::XMFLOAT2 pos;	//x,y,z
		DirectX::XMFLOAT2 uv;	//u,v
	};

struct PNTVertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 textureCoordinate;
};

struct Vertex2D {
    DirectX::XMFLOAT3 position;
};

static D3D11_INPUT_ELEMENT_DESC PNTVertexLayout[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

static D3D11_INPUT_ELEMENT_DESC Vertex2DLayout[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

static D3D11_INPUT_ELEMENT_DESC VideoVertex2DLayout[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0}
};


struct ConstantBufferNeverChanges
{
    DirectX::XMFLOAT4 lightPosition[4];
    DirectX::XMFLOAT4 lightColor;
};

struct ConstantBufferChangeOnResize
{
    DirectX::XMFLOAT4X4 projection;
};

struct ConstantBufferChangesEveryFrame
{
    DirectX::XMFLOAT4X4 view;
};

struct ConstantBufferChangesEveryPrim
{
    DirectX::XMFLOAT4X4 worldMatrix;
    DirectX::XMFLOAT4 meshColor;
    DirectX::XMFLOAT4 diffuseColor;
    DirectX::XMFLOAT4 specularColor;
    float specularPower;
};

//////////////////////////////////////
// Box2Dオブジェクト表示用
//////////////////////////////////////

struct BodyInfo 
{
	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT2 rotation;
	DirectX::XMFLOAT4 color;
};

struct ScreenInfo
{
  DirectX::XMFLOAT2 offset;
  DirectX::XMFLOAT2 scale;
  DirectX::XMFLOAT2 aspect;

};

