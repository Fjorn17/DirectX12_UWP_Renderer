#pragma once

#define USE_PIX
#define USE_PIC_SUPPORTED_ARCHITECTURE

#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <pix3.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <agile.h>
#include <concrt.h>

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif
