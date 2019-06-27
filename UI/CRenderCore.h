/*
Render core with DirectX 12
Engine for render 2D and 3D images
*/

#pragma once
#define WIN32_LEAN_AND_MEAN
#include <winapifamily.h>
#include <d3d12.h>
//DXGI interface
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d12sdklayers.h>
//Helper header
#include "d3dx12.h"
//Intelegent pointer
#include <wrl/client.h>
#include <string>
#include <sstream>
#include <iomanip>
//exception
#include <stdexcept>
//time
#include <chrono>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

#pragma comment(lib,"d3d12")
#pragma comment(lib,"dxgi")
#pragma comment(lib,"d3dcompiler")


namespace Ashr
{
	template <class ValueType>
	inline string Val2Str(ValueType val)
	{
		stringstream ss;
		ss << "<_" << val << "_>";
		return ss.str();
	}

	template <class ValueType>
	inline void ThrowError(ValueType errorVal)
	{
		if (FAILED(errorVal))
			throw(runtime_error::runtime_error(Val2Str(errorVal)));
	}

	namespace Render
	{
		class CRenderCore
		{
		public:
			CRenderCore();
			virtual ~CRenderCore();

		public:
			void Initialize(HWND hWnd);
			void Update();
			void Render();
			void Destroy();

		private:
			void LoadResource();
			void CreatePipeline(HWND hWnd);
			void CreateAssets();
			//Generate programe texture
			vector<UINT8> ProgTextureGen(UINT TextureWidth=256, UINT TexturePixelSize=4, UINT TextureHeight=256);
			//Frame process
			void MoveToNextFrm();
			void WaitForGPU();
			void PopulateCmdList();
			//Check support of variable frame rate
			bool CheckSupportTearing();

		private:
			//
			struct Vex
			{
				XMFLOAT3 pos;
				XMFLOAT2 uv;
			};

			//Pipeline object
			//Set frame buffer count
			static const UINT mFrmCount = 3;
			//Factory interface
			ComPtr<IDXGIFactory7> mpFactory;
			//Device interface
			ComPtr<ID3D12Device5> mpDevice;
			//Command queue
			ComPtr<ID3D12CommandQueue> mpCmdQueue;
			//Swapchain
			ComPtr<IDXGISwapChain4> mpSwapChain;
			//Descriptor heap to store render target view back buffer
			ComPtr<ID3D12DescriptorHeap> mpDescheap;
			//Descriptor heap size is vendor specific
			UINT mDescheapSize;
			//Render target view back buffer list according setted frame buffer count
			ComPtr<ID3D12Resource> mpRTVList[mFrmCount];
			//Root signature
			ComPtr<ID3D12RootSignature> mpRootSignature;
			//Pipeline state
			ComPtr<ID3D12PipelineState> mpPipelinestate;
			//Command allocator to store command list bundle for each frame
			ComPtr<ID3D12CommandAllocator> mpCmdAllocator[mFrmCount];
			//Command list
			ComPtr<ID3D12GraphicsCommandList4> mpCmdList;

			//App resource

			//Sychroniziation objects
			//Current frame index
			UINT mFrmIdx;
			//Fence
			HANDLE mhFenceEvent;
			ComPtr<ID3D12Fence1> mpFence;
			UINT64 mpFenceVals[mFrmCount];

		};
	}
}


