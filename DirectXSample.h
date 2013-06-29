//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// This header defines helper utilities to make DirectX APIs work with exceptions.
    #define ThrowIfFailed(hr) \
    {\
        if (FAILED(hr))\
        {\
            DOUT(hr);\
            throw Platform::Exception::CreateException(hr);\
        }\
    }
