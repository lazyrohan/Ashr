/*
Render core with DirectX 12
Engine for render 2D and 3D images
*/

#pragma once
#define WIN32_LEAN_AND_MEAN
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
#include <stdexcept>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

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

		private:
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
			//Current frame index
			UINT mFrmIdx;
			//Descriptor heap 
			ComPtr<ID3D12DescriptorHeap> mpDescheap;
			//Descriptor heap size
			UINT mDescheapSize;
			//Render target view list accoring setted frame buffer count
			ComPtr<ID3D12Resource> mpRTVList[mFrmCount];
			//Root signature
			ComPtr<ID3D12RootSignature> mpRootSignature;
			//Command allocator to store command list bundle
			ComPtr<ID3D12CommandAllocator> mpCmdAllocator;
			//Command list
			ComPtr<ID3D12CommandList> mpCmdList;


		};
	}
}


