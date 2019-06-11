#include "CRenderCore.h"

Ashr::Render::CRenderCore::CRenderCore() :
	mFrmIdx(0),
	mDescheapSize(0),
	mhFenceEvent(NULL),
	mpFenceVals{0}
{
}

Ashr::Render::CRenderCore::~CRenderCore()
{
	Destroy();
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
	// Record all the commands we need to render the scene into the command list.
	PopulateCmdList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { mpCmdList.Get() };
	mpCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	ThrowError(mpSwapChain->Present(1, 0));

	MoveToNextFrm();
}

void Ashr::Render::CRenderCore::Destroy()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
   // cleaned up by the destructor.
	WaitForGPU();

	CloseHandle(mhFenceEvent);
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
	fullscreenDesc.Windowed = false;
	fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	fullscreenDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
	fullscreenDesc.RefreshRate.Numerator = 60;
	fullscreenDesc.RefreshRate.Denominator = 1;


	ComPtr<IDXGISwapChain1> pSwapchain;
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#endif
	ThrowError(mpFactory->CreateSwapChainForHwnd(mpCmdQueue.Get(), hWnd, &mpSwapchainDesc, &fullscreenDesc, nullptr, &pSwapchain));

	//Shutdown fullscreen support
	//ThrowError(mpFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

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
	ThrowError(mpDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mpCmdAllocator[mFrmIdx])));

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
	{
		//Feature data
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		//Check supported version
		if (FAILED(mpDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		//Descriptor range
		CD3DX12_DESCRIPTOR_RANGE1 descRange[1] = { {} };
		descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

		CD3DX12_ROOT_PARAMETER1 rootPam[1] = { {} };
		rootPam[0].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

		D3D12_STATIC_SAMPLER_DESC staticSampler = {};
		staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		//Offset from the calculated mipmap level
		staticSampler.MipLODBias = 0;
		//For filter type,Valid values are between 1 and 16
		staticSampler.MaxAnisotropy = 0;
		staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		//0 is most detailed
		staticSampler.MinLOD = 0.0f;
		//no upper limit on LOD
		staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
		//Binding to HLSL
		staticSampler.ShaderRegister = 0;
		staticSampler.RegisterSpace = 0;
		staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Init_1_1(_countof(rootPam), rootPam, 1, &staticSampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> pSignature;
		ComPtr<ID3DBlob> pError;

		ThrowError(D3DX12SerializeVersionedRootSignature(&rootSigDesc, featureData.HighestVersion, &pSignature, &pError));
		ThrowError(mpDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&mpRootSignature)));
	}

	//Create pipeline state include loading and compiling shader
	{
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;
		ComPtr<ID3DBlob> pErrorMsg;

		UINT compileFlag = 0;
#if defined (_DEBUG)
		compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		wstring shaderFileName = L"";
		ThrowError(D3DCompileFromFile(shaderFileName.c_str(), nullptr, nullptr, "VSMain", "VS_5_0", compileFlag, 0, &vertexShader, &pErrorMsg));
		ThrowError(D3DCompileFromFile(shaderFileName.c_str(), nullptr, nullptr, "PSMain", "PS_5_0", compileFlag, 0, &pixelShader, &pErrorMsg));

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
		psoDesc.InputLayout = { inputEleDesc,_countof(inputEleDesc) };
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//number of render target format
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 1;
		psoDesc.NodeMask = 0;
		//psoDesc.CachedPSO =  ;
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		ThrowError(mpDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mpPipelinestate)));
	}

	//Create command list
	ThrowError(mpDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mpCmdAllocator[mFrmIdx].Get(), mpPipelinestate.Get(), IID_PPV_ARGS(&mpCmdList)));

	//Create vertex buffer/
	{
		//Define shape
		float m_aspectRatio = 0.2f;
		Vex triVex[] =
		{
		{{0.0f, 0.25f * m_aspectRatio, 0.0f},{0.5f, 0.0f}},
		{{0.25f, -0.25f * m_aspectRatio, 0.0f},{1.0f, 1.0f}},
		{{-0.25f, -0.25f * m_aspectRatio, 0.0f},{0.0f, 1.0f}}
		};

		D3D12_HEAP_PROPERTIES heapPro = {};
		//Max GPU bandwidth but no CPU write
		heapPro.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapPro.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
		heapPro.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapPro.CreationNodeMask = 0;
		heapPro.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = 1080;
		resDesc.Height = 800;
		resDesc.DepthOrArraySize = sizeof(triVex);
		resDesc.MipLevels = 0;
		resDesc.Format = DXGI_FORMAT_D32_FLOAT;
		resDesc.SampleDesc.Count = 4;
		resDesc.SampleDesc.Quality = 1;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ComPtr<ID3D12Resource1> pVexBuf;
		ThrowError(mpDevice->CreateCommittedResource1(&heapPro,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, nullptr,
			IID_PPV_ARGS(&pVexBuf)));

		//Load shape data to vertex buffer
		D3D12_RANGE readRange = { 0,0 };
		UINT8* pVexdataStart = 0;

		ThrowError(pVexBuf->Map(0, &readRange, reinterpret_cast<void**>(&pVexdataStart)));
		memcpy(pVexdataStart, triVex, sizeof(triVex));
		pVexBuf->Unmap(0, nullptr);

		//Initialize vertex buffer view
		D3D12_VERTEX_BUFFER_VIEW vexBufView = {};
		vexBufView.BufferLocation = pVexBuf->GetGPUVirtualAddress();
		vexBufView.SizeInBytes = sizeof(triVex);
		vexBufView.StrideInBytes = sizeof(Vex);

	}

	//Create texture 
	{
		// Describe and create a Texture2D.
		UINT TextureWidth = 256u;
		UINT TextureHeight = 256u;
		UINT TexturePixelSize = 4u;

		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = TextureWidth;
		textureDesc.Height = TextureHeight;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		D3D12_HEAP_PROPERTIES texHeapPro = {};
		texHeapPro.Type = D3D12_HEAP_TYPE_DEFAULT;
		texHeapPro.CreationNodeMask = 0;
		texHeapPro.VisibleNodeMask = 0;
		texHeapPro.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		texHeapPro.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

		ComPtr<ID3D12Resource1> pTexture;
		ThrowError(mpDevice->CreateCommittedResource(
			&texHeapPro,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&pTexture)));

		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(pTexture.Get(), 0, 1);

		// Create the GPU upload buffer.
		ComPtr<ID3D12Resource1> textureUploadHeap;
		ThrowError(mpDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureUploadHeap)));

		// Copy data to the intermediate upload heap and then schedule a copy 
		// from the upload heap to the Texture2D.
		std::vector<UINT8> texture = ProgTextureGen();

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = &texture[0];
		textureData.RowPitch = TextureWidth * TexturePixelSize;
		textureData.SlicePitch = textureData.RowPitch * TextureHeight;

		UpdateSubresources(mpCmdList.Get(), pTexture.Get(), textureUploadHeap.Get(), 0, 0, 1, &textureData);
		mpCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		ComPtr<ID3D12DescriptorHeap> pTexSRVHeap;
		mpDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, pTexSRVHeap->GetCPUDescriptorHandleForHeapStart());
	}

	// Close command list and initial GUP setup
	{
	ThrowError(mpCmdList->Close());

	ID3D12CommandList* pCmdlists[] = { mpCmdList.Get() };
	mpCmdQueue->ExecuteCommandLists(_countof(pCmdlists), pCmdlists);
	}

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{

		ThrowError(mpDevice->CreateFence(mpFenceVals[mFrmIdx], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpFence)));
		mpFenceVals[mFrmIdx]++;

		//Create event to handle frame synchronization
		CreateEventW(nullptr, false, false, nullptr);

		if (mhFenceEvent == nullptr)
		{
			ThrowError(GetLastError());
		}

		WaitForGPU();
	}

}

vector<UINT8> Ashr::Render::CRenderCore::ProgTextureGen(UINT TextureWidth, UINT TexturePixelSize, UINT TextureHeight)
{
	const UINT rowPitch = TextureWidth * TexturePixelSize;
	const UINT cellPitch = rowPitch >> 3;        // The width of a cell in the checkboard texture.
	const UINT cellHeight = TextureWidth >> 3;    // The height of a cell in the checkerboard texture.
	const UINT textureSize = rowPitch * TextureHeight;

	std::vector<UINT8> data(textureSize);
	UINT8* pData = &data[0];

	for (UINT n = 0; n < textureSize; n += TexturePixelSize)
	{
		UINT x = n % rowPitch;
		UINT y = n / rowPitch;
		UINT i = x / cellPitch;
		UINT j = y / cellHeight;

		if (i % 2 == j % 2)
		{
			pData[n] = 0x00;        // R
			pData[n + 1] = 0x00;    // G
			pData[n + 2] = 0x00;    // B
			pData[n + 3] = 0xff;    // A
		}
		else
		{
			pData[n] = 0xff;        // R
			pData[n + 1] = 0xff;    // G
			pData[n + 2] = 0xff;    // B
			pData[n + 3] = 0xff;    // A
		}
	}

	return data;
}

void Ashr::Render::CRenderCore::MoveToNextFrm()
{
	// Schedule a Signal command in the queue.
	const UINT64 currentFenceValue = mpFenceVals[mFrmIdx];
	ThrowError(mpCmdQueue->Signal(mpFence.Get(), currentFenceValue));

	// Update the frame index.
	mFrmIdx= mpSwapChain->GetCurrentBackBufferIndex();

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (mpFence->GetCompletedValue() < mpFenceVals[mFrmIdx])
	{
		ThrowError(mpFence->SetEventOnCompletion(mpFenceVals[mFrmIdx], mhFenceEvent));
		WaitForSingleObjectEx(mhFenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	mpFenceVals[mFrmIdx] = currentFenceValue + 1;
}

void Ashr::Render::CRenderCore::WaitForGPU()
{
	// Schedule a Signal command in the queue.
	ThrowError(mpCmdQueue->Signal(mpFence.Get(), mpFenceVals[mFrmIdx]));

	// Wait until the fence has been processed.
	ThrowError(mpFence->SetEventOnCompletion(mpFenceVals[mFrmIdx], mhFenceEvent));
	WaitForSingleObjectEx(mhFenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	mpFenceVals[mFrmIdx]++;
}

void Ashr::Render::CRenderCore::PopulateCmdList()
{
	// Command list allocators can only be reset when the associated 
  // command lists have finished execution on the GPU; apps should use 
  // fences to determine GPU execution progress.
	ThrowError(mpCmdAllocator[mFrmIdx]->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowError(mpCmdList->Reset(mpCmdAllocator[mFrmIdx].Get(), mpPipelinestate.Get()));

	// Set necessary state.
	mpCmdList->SetGraphicsRootSignature(mpRootSignature.Get());
	D3D12_VIEWPORT mViewport;
	D3D12_RECT mRect;
	mpCmdList->RSSetViewports(1, &mViewport);
	mpCmdList->RSSetScissorRects(1, &mRect);

	// Indicate that the back buffer will be used as a render target.
	mpCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mpRTVList[mFrmIdx].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mpDescheap->GetCPUDescriptorHandleForHeapStart(), mFrmIdx, mDescheapSize);
	mpCmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	mpCmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	mpCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//mpCmdList->IASetVertexBuffers(0, 1, &);
	mpCmdList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will now be used to present.
	mpCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mpRTVList[mFrmIdx].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowError(mpCmdList->Close());
}
