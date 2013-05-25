//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <wrl.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d2d1effects.h>
#include <d2d1effecthelpers.h>

#include <dwrite_1.h>
#include <wincodec.h>
#include <DirectXMath.h>

#include <xaudio2.h>
#include <xaudio2fx.h>

#include <mmreg.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
//#include <dcomp.h>

#include <wincodecsdk.h>

#include <XInput.h>

#include <stdio.h>
#include <vector>
#include <memory>
#include <array>
#include <boost\format.hpp>

#include <ppltasks.h>
#include <agile.h>
#include <Box2D.h>

#define _WRL_PTR_TYPEDEF(x) typedef Microsoft::WRL::ComPtr<x> x ## Ptr

#define CHK(statement)	{HRESULT _hr = (statement); if (FAILED(_hr)) { throw ref new Platform::COMException(_hr); };}

