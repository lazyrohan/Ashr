#include "CRenderCore.h"

Ashr::Render::CRenderCore::CRenderCore()
{
}

Ashr::Render::CRenderCore::~CRenderCore()
{
}

void Ashr::Render::CRenderCore::Initialize(HWND hWnd)
{
	CreatePipeline(hWnd);
	CreateAssets();
}

void Ashr::Render::CRenderCore::Update()
{
}

void Ashr::Render::CRenderCore::Render()
{
}

void Ashr::Render::CRenderCore::Destroy()
{
}

void Ashr::Render::CRenderCore::LoadResource()
{
}

void Ashr::Render::CRenderCore::CreatePipeline(HWND hWnd)
{
	/*
• Initialize the pipeline.

	• Enable the debug layer.
	• Create the device.
	• Create the command queue.
	• Create the swap chain.
	• Create a render target view(RTV) descriptor heap.
	Note
	A descriptor heap can be thought of as an array of descriptors.Where each descriptor fully describes an object to the GPU.

	• Create frame resources(a render target view for each frame).
	• Create a command allocator.
	Note
	A command allocator manages the underlying storage for command listsand bundles.
	*/

	//Enable debug in debug mold
	UINT dxgifactoryflags = 0;
#if defined (_DEBUG)
	{

		ComPtr<ID3D12Debug> debugger;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugger))))
		{
			debugger->EnableDebugLayer();
			dxgifactoryflags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	//Create factory
	ThrowError(CreateDXGIFactory2(dxgifactoryflags, IID_PPV_ARGS(&mpFactory)));

	//Create device
	ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterNum = 0; DXGI_ERROR_NOT_FOUND != mpFactory->EnumAdapters1(adapterNum, &adapter); ++adapterNum)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		adapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
		{
			continue;
		}
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device5), nullptr)))
		{
			break;
		}
	}

	ThrowError(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mpDevice)));

	//Create command queue
	D3D12_COMMAND_QUEUE_DESC cmdDesc = {};
	cmdDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;


	ThrowError(mpDevice->CreateCommandQueue(&cmdDesc, IID_PPV_ARGS(&mpCmdQueue)));

	//Create swapchain
	DXGI_SWAP_CHAIN_DESC1 mpSwapchainDesc = {};
	mpSwapchainDesc.BufferCount = mFrmCount;
	mpSwapchainDesc.Width = 1080;
	mpSwapchainDesc.Height = 800;
	mpSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	mpSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	mpSwapchainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	mpSwapchainDesc.SampleDesc.Count = 1;
	mpSwapchainDesc.SampleDesc.Quality = 4;
	mpSwapchainDesc.Scaling = DXGI_SCALING_NONE;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
	fullscreenDesc.Windowed = true;
	fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	fullscreenDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
	DXGI_RATIONAL ratio = {};
	ratio.Numerator = 60;
	ratio.Denominator = 1;
	fullscreenDesc.RefreshRate = ratio;


	ComPtr<IDXGISwapChain1> pSwapchain;
	ThrowError(mpFactory->CreateSwapChainForHwnd(mpCmdQueue.Get(), hWnd, &mpSwapchainDesc, &fullscreenDesc, nullptr, &pSwapchain));

	//Shutdown fullscreen support
	ThrowError(mpFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

	//Set to high level swapchain interface
	ThrowError(pSwapchain.As(&mpSwapChain));

	//Get current frame index
	mFrmIdx = mpSwapChain->GetCurrentBackBufferIndex();

	//Create render target descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeepDesc = {};
	rtvHeepDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeepDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeepDesc.NumDescriptors = mFrmCount;
	//multiple adapter identification
	rtvHeepDesc.NodeMask = 0;

	ThrowError(mpDevice->CreateDescriptorHeap(&rtvHeepDesc, IID_PPV_ARGS(&mpDescheap)));
	mpDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//Create frame resource
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mpDescheap->GetCPUDescriptorHandleForHeapStart();

	//Create render target view for each frame
	for (UINT i = 0; i < mFrmCount; ++i)
	{
		ThrowError(mpSwapChain->GetBuffer(i, IID_PPV_ARGS(&mpRTVList[i])));
		mpDevice->CreateRenderTargetView(mpRTVList[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += UINT64(mDescheapSize);
	}

	//Create command allocator
	ThrowError(mpDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mpCmdAllocator)));

}

void Ashr::Render::CRenderCore::CreateAssets()
{
	/*

	• Initialize the assets.

	• Create an empty root signature.
	Note
	A graphics root signature defines what resources are bound to the graphics pipeline.

	• Compile the shaders.
	• Create the vertex input layout.
	• Create a pipeline state object description, then create the object.
	Note
	A pipeline state object maintains the state of all currently set shaders as well as certain fixed function state objects(such as the input assembler, tesselator, rasterizerand output merger).

	• Create the command list.
	• Close the command list.
	• Createand load the vertex buffers.
	• Create the vertex buffer views.
	• Create a fence.
	Note
	A fence is used to synchronize the CPUand GPU.

	• Create an event handle.
	Wait for the GPU to finish.
	*/

	//Create root signature
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSigDesc.NumParameters = 0;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pParameters = nullptr;
	rootSigDesc.pStaticSamplers = nullptr;

	ComPtr<ID3DBlob> pSignature;
	ComPtr<ID3DBlob> pError;

	ThrowError(D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &pSignature, &pError));
	ThrowError(mpDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&mpRootSignature)));

	//Create pipeline state include loading and compiling shader
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
	//ComPtr<ID3DBlob> pErrorMsg;

	UINT compileFlag = 0;
#if defined (_DEBUG)
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	wstring shaderFileName = L"";
	ThrowError(D3DCompileFromFile(shaderFileName.c_str(), nullptr, nullptr, "VSMain", "VS_5_0", compileFlag, 0, &vertexShader, &pError));
	ThrowError(D3DCompileFromFile(shaderFileName.c_str(), nullptr, nullptr, "PSMain", "PS_5_0", compileFlag, 0, &pixelShader, &pError));

	//Define vertex input layout
	D3D12_INPUT_ELEMENT_DESC inputEleDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
	inputEleDesc[0].SemanticName = "POSITION";
	inputEleDesc[0].SemanticIndex = 0;
	inputEleDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputEleDesc[0].InputSlot = 0;
	inputEleDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputEleDesc[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputEleDesc[0].InstanceDataStepRate = 0;

	//Create graphic pipeline state object
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = mpRootSignature.Get();
	/*
	D3D12_SHADER_BYTECODE vcode = {};
	vcode.pShaderBytecode = vertexShader.Get();
	vcode.BytecodeLength = sizeof(vertexShader);
	*/
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
	psoDesc.DS = CD3DX12_SHADER_BYTECODE();
	psoDesc.HS = CD3DX12_SHADER_BYTECODE();
	psoDesc.GS = CD3DX12_SHADER_BYTECODE();
	psoDesc.StreamOutput = D3D12_STREAM_OUTPUT_DESC();
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.DepthStencilState.StencilEnable = false;

	//Create command list
	ThrowError(mpDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mpCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&mpCmdList)));

}
