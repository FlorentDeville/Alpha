/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "RenderModule.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

#include "CommandQueue.h"
#include "Core/Helper.h"

#include "Rendering/BaseShape.h"
#include "Rendering/Camera.h"
#include "Rendering/ConstantBuffer/LinearConstantBufferPool.h"
#include "Rendering/Font/Font.h"
#include "Rendering/Font/FontMgr.h"
#include "Rendering/Material/Material.h"
#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"

#include "Rendering/Shaders/Shader.h"
#include "Rendering/Shaders/ShaderMgr.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureMgr.h"

#include "InputLayout/InputLayout.h"

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif

namespace Rendering
{
	RenderModule::RenderModule()
		: m_pDevice(nullptr)
		, m_pSwapChain(nullptr)
		, m_pRTVHeap(nullptr)
		, m_pDSVHeap(nullptr)
		, m_allowTearing(false)
		, m_pRenderCommandQueue(nullptr)
		, m_pCopyCommandQueue(nullptr)
		, m_currentBackBufferIndex(0)
		, m_vSync(true)
		, m_pMainDepthBuffer(nullptr)
#if defined(_DEBUG)
		, m_pDebugInterface(nullptr)
#endif
		, m_pRenderCommandList(nullptr)
		, m_pCamera(nullptr)
		, m_pCubeMesh(nullptr)
		, m_pLinearCBufferPool(nullptr)
	{
		m_clearColor[0] = 0.4f;
		m_clearColor[1] = 0.6f;
		m_clearColor[2] = 0.9f;
		m_clearColor[3] = 1.0f;
	}

	RenderModule::~RenderModule()
	{}

	void RenderModule::Init(HWND hWindow, const DirectX::XMUINT2& gameResolution, const DirectX::XMUINT2& mainResolution, const std::string& binPath)
	{
		m_gameResolution = gameResolution;
		m_mainResolution = mainResolution;

		Rendering::ShaderMgr::InitSingleton();
		Rendering::RootSignatureMgr::InitSingleton();
		Rendering::MeshMgr::InitSingleton();
		Rendering::MaterialMgr::InitSingleton();
		Rendering::PipelineStateMgr::InitSingleton();
		Rendering::FontMgr::InitSingleton();
		Rendering::TextureMgr::InitSingleton();

		EnableDebugLayer();

		CheckTearingSupport();

		IDXGIAdapter4* pAdapter = GetAdapter(false);
		CreateDevice(pAdapter);
		pAdapter->Release();

		m_pRenderCommandQueue = new CommandQueue(m_pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_pCopyCommandQueue = new CommandQueue(m_pDevice, D3D12_COMMAND_LIST_TYPE_COPY);

		CreateSwapChain(hWindow, m_pRenderCommandQueue->GetD3D12CommandQueue(), mainResolution.x, mainResolution.y, m_numFrames);
		m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		m_pRTVHeap = CreateRTVHeap();
		for (int ii = 0; ii < m_numFrames; ++ii)
		{
			m_mainRTV[ii] = m_pRTVHeap->GetNewHandle();
		}

		m_pDSVHeap = CreateDSVHeap();
		m_mainDSV = m_pDSVHeap->GetNewHandle();

		UpdateRenderTargetViews();

		ResizeDepthBuffer(mainResolution.x, mainResolution.y, &m_pMainDepthBuffer, m_mainDSV);

		m_pLinearCBufferPool = new LinearConstantBufferPool();
		const int CBUFFER_SIZE = 512;
		const int CBUFFER_COUNT = 100;
		m_pLinearCBufferPool->Init(CBUFFER_SIZE, CBUFFER_COUNT);

		//Render target for the game
		m_gameRenderTarget = CreateRenderTarget(gameResolution.x, gameResolution.y);

		m_pCamera = new Camera();

		Rendering::MeshMgr& meshMgr = Rendering::MeshMgr::Get();

		Rendering::MeshId cylinderMeshId;
		meshMgr.CreateMesh(&m_pCylinderMesh, cylinderMeshId);
		BaseShape::CreateCylinder(m_pCylinderMesh, 1, 1, 40);

		Rendering::MeshId coneMeshId;
		meshMgr.CreateMesh(&m_pConeMesh, coneMeshId);
		BaseShape::CreateCone(m_pConeMesh, 1, 1, 40);

		MeshId torusMeshId;
		meshMgr.CreateMesh(&m_pTorusMesh, torusMeshId);
		BaseShape::CreateTorus(m_pTorusMesh, 2, 0.02f, 40);

		MeshId cubeMeshId;
		meshMgr.CreateMesh(&m_pCubeMesh, cubeMeshId);
		BaseShape::CreateCube(m_pCubeMesh);

		//basic shape material (should be an app resources?)
		{
			Rendering::ShaderMgr& shaderMgr = Rendering::ShaderMgr::Get();
			Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

			const std::string shaderPath = binPath + "\\";

			Rendering::RootSignatureId rsId = Rendering::RootSignatureMgr::Get().CreateRootSignature(shaderPath + "base-shape.rs.cso");
			Rendering::ShaderId vsId = shaderMgr.CreateShader(shaderPath + "base-shape.vs.cso");
			Rendering::ShaderId psId = shaderMgr.CreateShader(shaderPath + "base-shape.ps.cso");

			Rendering::PipelineStateId pid;
			Rendering::PipelineState* pPipelineState = Rendering::PipelineStateMgr::Get().CreatePipelineState(pid);
			pPipelineState->Init_Generic(rsId, vsId, psId);

			Rendering::Material* pMaterial = nullptr;
			Rendering::MaterialId materialId;
			materialMgr.CreateMaterial(&pMaterial, materialId);
			pMaterial->Init(rsId, pid);

			m_pBaseShapeMaterial = pMaterial;
		}

	}

	void RenderModule::Release()
	{
		for (std::pair<Rendering::FontId, FontRenderInfo> it : m_fontVertexBuffers)
		{
			for (int ii = 0; ii < 3; ++ii)
			{
				ID3D12Resource* pResource = it.second.m_textVertexBuffer[ii];
				if (pResource)
					pResource->Release();
			}
		}

		delete m_gameRenderTarget;

		Rendering::FontMgr::ReleaseSingleton();
		Rendering::MaterialMgr::ReleaseSingleton();
		Rendering::MeshMgr::ReleaseSingleton();
		Rendering::RootSignatureMgr::ReleaseSingleton();
		Rendering::PipelineStateMgr::ReleaseSingleton();
		Rendering::ShaderMgr::ReleaseSingleton();
		Rendering::TextureMgr::ReleaseSingleton();

		m_pRenderCommandQueue->Flush();
		m_pCopyCommandQueue->Flush();

		delete m_pRenderCommandQueue;
		delete m_pCopyCommandQueue;
		delete m_pCamera;

#if defined(_DEBUG)
		m_pDebugInterface->Release();
#endif

		if (m_pLinearCBufferPool)
		{
			delete m_pLinearCBufferPool;
			m_pLinearCBufferPool = nullptr;
		}

		for (ID3D12Resource* pBackBuffer : m_pBackBuffers)
		{
			if (pBackBuffer)
				pBackBuffer->Release();
		}

		if (m_pMainDepthBuffer)
			m_pMainDepthBuffer->Release();

		if (m_pDSVHeap)
		{
			m_pDSVHeap->Release();
			delete m_pDSVHeap;
		}

		if (m_pRTVHeap)
		{
			m_pRTVHeap->Release();
			delete m_pRTVHeap;
		}

		m_pSwapChain->Release();
		m_pDevice->Release();

		ReportLiveObject();
	}

	void RenderModule::PreRender()
	{
		//new render command list
		m_pRenderCommandList = m_pRenderCommandQueue->GetCommandList();

		m_pLinearCBufferPool->FreeAll();
	}

	void RenderModule::PostRender()
	{ }

	void RenderModule::BeginMainScene()
	{
		ID3D12Resource* pBackBuffer = m_pBackBuffers[m_currentBackBufferIndex];

		// Clear the render target.
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			m_pRenderCommandList->ResourceBarrier(1, &barrier);
		}

		FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
		m_pRenderCommandList->ClearRenderTargetView(m_mainRTV[m_currentBackBufferIndex], clearColor, 0, nullptr);


		// Clear the depth buffer
		float depthValue = 1.f;
		m_pRenderCommandList->ClearDepthStencilView(m_mainDSV, D3D12_CLEAR_FLAG_DEPTH, depthValue, 0, 0, nullptr);

		//Set viewport and scissors
		m_pRenderCommandList->RSSetViewports(1, &m_mainViewport);
		m_pRenderCommandList->RSSetScissorRects(1, &m_mainScissorRect);

		// Set render targets
		m_pRenderCommandList->OMSetRenderTargets(1, &m_mainRTV[m_currentBackBufferIndex], FALSE, &m_mainDSV);
	}

	void RenderModule::EndMainScene()
	{
		//final command : switch the back buffer to present state
		ID3D12Resource* pBackBuffer = m_pBackBuffers[m_currentBackBufferIndex];
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(pBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			m_pRenderCommandList->ResourceBarrier(1, &barrier);
		}
	}

	void RenderModule::BindMaterial(const Rendering::Material& material, const DirectX::XMMATRIX& wvp)
	{
		m_pRenderCommandList->SetPipelineState(material.m_pPipelineState->GetPipelineState());
		m_pRenderCommandList->SetGraphicsRootSignature(material.m_pRootSignature->GetRootSignature());

		m_pRenderCommandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &wvp, 0);

		if (material.m_textureId != Rendering::TextureId::INVALID)
		{
			Rendering::Texture* pTexture = Rendering::TextureMgr::Get().GetTexture(material.m_textureId);
			ID3D12DescriptorHeap* pSrv = pTexture->GetSRV();
			ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
			m_pRenderCommandList->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
			m_pRenderCommandList->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());
		}
	}

	void RenderModule::BindMaterial(const PipelineState& pso, const RootSignature& rs)
	{
		m_pRenderCommandList->SetPipelineState(pso.GetPipelineState());
		m_pRenderCommandList->SetGraphicsRootSignature(rs.GetRootSignature());
	}

	void RenderModule::BindMaterial(const PipelineState& pso, const RootSignature& rs, const DirectX::XMMATRIX& wvp)
	{
		m_pRenderCommandList->SetPipelineState(pso.GetPipelineState());
		m_pRenderCommandList->SetGraphicsRootSignature(rs.GetRootSignature());

		m_pRenderCommandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &wvp, 0);
	}

	void RenderModule::BindCBuffer(uint32_t rootParamIndex, int poolIndex)
	{
		m_pRenderCommandList->SetGraphicsRootConstantBufferView(rootParamIndex, m_pLinearCBufferPool->GetGpuAddress(poolIndex));
	}

	void RenderModule::RenderMesh(const Rendering::Mesh& mesh)
	{
		m_pRenderCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pRenderCommandList->IASetVertexBuffers(0, 1, &mesh.GetVertexBufferView());
		m_pRenderCommandList->IASetIndexBuffer(&mesh.GetIndexBufferView());

		m_pRenderCommandList->DrawIndexedInstanced(mesh.GetIndicesCount(), 1, 0, 0, 0);
	}

	void RenderModule::RenderPrimitiveCylinder(const DirectX::XMMATRIX& world, const DirectX::XMFLOAT4& color)
	{
		RenderBaseShape(m_pCylinderMesh, world, color);
	}

	void RenderModule::RenderPrimitiveCone(const DirectX::XMMATRIX& world, const DirectX::XMFLOAT4& color)
	{
		RenderBaseShape(m_pConeMesh, world, color);
	}

	void RenderModule::RenderPrimitiveTorus(const DirectX::XMMATRIX& world, const DirectX::XMFLOAT4& color)
	{
		RenderBaseShape(m_pTorusMesh, world, color);
	}

	void RenderModule::RenderPrimitiveCube(const DirectX::XMMATRIX& world, const DirectX::XMFLOAT4& color)
	{
		RenderBaseShape(m_pCubeMesh, world, color);
	}

	void RenderModule::ExecuteRenderCommand()
	{
		//run the command list
		uint64_t fenceValue = m_pRenderCommandQueue->ExecuteCommandList(m_pRenderCommandList);

		//present
		UINT syncInterval = m_vSync ? 1 : 0;
		UINT presentFlags = m_allowTearing && !m_vSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		ThrowIfFailed(m_pSwapChain->Present(syncInterval, presentFlags));

		//wait for the commands to be run
		m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		m_pRenderCommandQueue->WaitForFenceValue(fenceValue);
		m_pRenderCommandList = nullptr;
	}

	void RenderModule::SetConstantBuffer(int32_t registerId, int32_t sizeInBytes, void* pData, int32_t offset)
	{
		m_pRenderCommandList->SetGraphicsRoot32BitConstants(registerId, sizeInBytes / 4, pData, offset);
	}

	void RenderModule::ResizeSwapChain(uint32_t width, uint32_t height)
	{
		m_mainResolution = DirectX::XMUINT2(width, height);

		// Flush the GPU queue to make sure the swap chain's back buffers
		// are not being referenced by an in-flight command list.
		m_pCopyCommandQueue->Flush();
		m_pRenderCommandQueue->Flush();

		for (int ii = 0; ii < m_numFrames; ++ii)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			m_pBackBuffers[ii]->Release();
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ThrowIfFailed(m_pSwapChain->GetDesc(&swapChainDesc));
		ThrowIfFailed(m_pSwapChain->ResizeBuffers(m_numFrames, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

		m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		UpdateRenderTargetViews();

		m_mainViewport = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(width), static_cast<float>(height));

		char buffer[500];
		sprintf_s(buffer, 500, "Resized swap chain %d %d\n", width, height);
		OutputDebugString(buffer);
	}

	void RenderModule::ResizeDepthBuffer(uint32_t width, uint32_t height, ID3D12Resource** pResource, D3D12_CPU_DESCRIPTOR_HANDLE dsv)
	{
		// Flush any GPU commands that might be referencing the depth buffer.
		m_pCopyCommandQueue->Flush();
		m_pRenderCommandQueue->Flush();

		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		if (*pResource)
			(*pResource)->Release();

		ThrowIfFailed(m_pDevice->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(pResource)
		));

		// Update the depth-stencil view.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_pDevice->CreateDepthStencilView(*pResource, &dsvDesc, dsv);

		char buffer[500];
		sprintf_s(buffer, 500, "Resized depth buffer %d %d\n", width, height);
		OutputDebugString(buffer);

	}

	int RenderModule::GetNumFrames() const
	{
		return m_numFrames;
	}

	const DirectX::XMUINT2& RenderModule::GetGameResolution() const
	{
		return m_gameResolution;
	}

	Camera* RenderModule::GetCamera()
	{
		return m_pCamera;
	}

	const Camera* RenderModule::GetConstCamera() const
	{
		return m_pCamera;
	}

	void RenderModule::InitialiseFont(Rendering::FontId fontId, Rendering::PipelineStateId psoId, int maxCharacterCount)
	{
		FontRenderInfo& info = m_fontVertexBuffers[fontId];
		info.m_bufferSize = maxCharacterCount;
		info.m_characterCount = 0;
		info.m_psoId = psoId;

		for (int ii = 0; ii < m_numFrames; ++ii)
		{
			// create upload heap. We will fill this with data for our text
			D3D12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(maxCharacterCount * sizeof(Rendering::VertexText));
			HRESULT hr = RenderModule::Get().GetDevice()->CreateCommittedResource(
				&prop, // upload heap
				D3D12_HEAP_FLAG_NONE, // no flags
				&desc, // resource description for a buffer
				D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
				nullptr,
				IID_PPV_ARGS(&info.m_textVertexBuffer[ii]));

			ThrowIfFailed(hr);
			info.m_textVertexBuffer[ii]->SetName(L"Text Vertex Buffer Upload Resource Heap");

			CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

			// map the resource heap to get a gpu virtual address to the beginning of the heap
			hr = info.m_textVertexBuffer[ii]->Map(0, &readRange, reinterpret_cast<void**>(&info.m_textVBGPUAddress[ii]));
			ThrowIfFailed(hr);

			info.m_textVertexBufferView[ii].BufferLocation = info.m_textVertexBuffer[ii]->GetGPUVirtualAddress();
			info.m_textVertexBufferView[ii].StrideInBytes = sizeof(Rendering::VertexText);
			info.m_textVertexBufferView[ii].SizeInBytes = maxCharacterCount * sizeof(Rendering::VertexText);
		}
	}

	int RenderModule::PrepareRenderText(const std::string& text, Rendering::FontId fontId, const DirectX::XMFLOAT3& uiPos, const DirectX::XMFLOAT2& scale, const DirectX::XMUINT4& scissor,
		float nearCameraPlane, float farCameraPlane)
	{
		FontRenderInfo& info = m_fontVertexBuffers[fontId];
		const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(fontId);// m_fontMgr.GetResource(fontId);

		//convert from ui coordinates to screen coordinate
		// ui coordinate : from the top left corner in pixels
		// screen coordinate : from the center, range [-1, 1]
		float startingX = (uiPos.x * 2 / m_mainResolution.x) - 1;
		float y = 2 - (uiPos.y * 2 / m_mainResolution.y) - 1;

		float scissorXMax = static_cast<float>(scissor.x + scissor.z);

		// cast the gpu virtual address to a textvertex, so we can directly store our vertices there
		Rendering::VertexText* vert = (Rendering::VertexText*)info.m_textVBGPUAddress[m_currentBackBufferIndex];

		char lastChar = -1; // no last character to start with

		float xInScreenSpace = startingX;
		float xInPixel = uiPos.x;

		for (int i = 0; i < text.size(); ++i)
		{
			char c = text[i];

			const Rendering::FontChar* fc = pFont->GetChar(c);

			// character not in font char set
			if (fc == nullptr)
			{
				OutputDebugString("WARNING : Character not found in the font\n");
				break;
			}

			// end of string
			if (c == '\0')
				break;

			// don't overflow the buffer. In your app if this is true, you can implement a resize of your text vertex buffer
			if (info.m_characterCount >= info.m_bufferSize)
			{
				OutputDebugString("WARNING : Too many characters rendered\n");
			}

			float kerning = 0.0f;
			if (i > 0)
			{
				//the kerning is in pixel but I need to convert it to screen unit [-1; 1]
				kerning = static_cast<float>(pFont->GetKerning(lastChar, c));
				kerning = kerning / (m_mainResolution.x * 2);
			}

			float xoffset = static_cast<float>(fc->m_xoffset) / m_mainResolution.x * 2;
			float yoffset = static_cast<float>(fc->m_yoffset) / m_mainResolution.y * 2;

			float char_width = static_cast<float>(fc->m_width) / m_mainResolution.x * 2.f;
			float char_height = static_cast<float>(fc->m_height) / m_mainResolution.y * 2.f;

			Rendering::VertexText& pVertexText = vert[info.m_characterCount];
			pVertexText.Position.x = xInScreenSpace + ((xoffset + kerning) * scale.x);
			pVertexText.Position.y = y - (yoffset * scale.y);
			pVertexText.Position.z = char_width * scale.x;
			pVertexText.Position.w = char_height * scale.y;
			pVertexText.Uv.x = fc->m_u;
			pVertexText.Uv.y = fc->m_v;
			pVertexText.Uv.z = fc->m_twidth;
			pVertexText.Uv.w = fc->m_theight;
			pVertexText.Color = DirectX::XMFLOAT4(1, 1, 1, 1);
			pVertexText.Z = uiPos.z / (farCameraPlane - nearCameraPlane);

			float advance = static_cast<float>(fc->m_xadvance) / m_mainResolution.x * 2;
			xInScreenSpace += advance * scale.x;

			xInPixel += fc->m_xadvance * scale.x;

			lastChar = c;

			//we are out of bounds, ignore the last character setup
			//do not do this test with the screen coordinate. I loose accuracy over time.
			if (xInPixel > scissorXMax)
				return i - 1;

			info.m_characterCount++;
		}

		return static_cast<int>(text.size());
	}

	void RenderModule::RenderAllText()
	{
		Rendering::TextureMgr& textureMgr = Rendering::TextureMgr::Get();

		for (auto& fontPair : m_fontVertexBuffers)
		{
			FontRenderInfo& info = fontPair.second;
			const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(fontPair.first);

			ID3D12GraphicsCommandList2* pCommandList = GetRenderCommandList();

			// set the text pipeline state object
			Rendering::PipelineState* pPipelineState = Rendering::PipelineStateMgr::Get().GetPipelineState(info.m_psoId);
			ID3D12PipelineState* pPSO = pPipelineState->GetPipelineState();
			pCommandList->SetPipelineState(pPSO);

			Rendering::RootSignature* pRootSignature = Rendering::RootSignatureMgr::Get().GetRootSignature(pPipelineState->GetRootSignatureId());
			pCommandList->SetGraphicsRootSignature(pRootSignature->GetRootSignature());

			// this way we only need 4 vertices per quad rather than 6 if we were to use a triangle list topology
			pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			// set the text vertex buffer
			pCommandList->IASetVertexBuffers(0, 1, &info.m_textVertexBufferView[m_currentBackBufferIndex]);

			// bind the text srv.
			ID3D12DescriptorHeap* pSrv = textureMgr.GetTexture(pFont->m_texture)->GetSRV();
			ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
			pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
			pCommandList->SetGraphicsRootDescriptorTable(0, pSrv->GetGPUDescriptorHandleForHeapStart());

			// we are going to have 4 vertices per character (trianglestrip to make quad), and each instance is one character
			pCommandList->DrawInstanced(4, info.m_characterCount, 0, 0);

			//reset the character count for next frame
			info.m_characterCount = 0;
		}
	}

	Rendering::TextureId RenderModule::GetGameRenderTargetTextureId() const
	{
		return GetRenderTargetTextureId(m_gameRenderTarget);
	}

	void RenderModule::ChangeMainResolution(const DirectX::XMUINT2& size)
	{
		ResizeSwapChain(size.x, size.y);
		ResizeDepthBuffer(size.x, size.y, &m_pMainDepthBuffer, m_mainDSV);
	}

	Rendering::RenderTarget* RenderModule::CreateRenderTarget(int width, int height)
	{
		return new Rendering::RenderTarget(width, height);
	}

	Rendering::DescriptorHeap* RenderModule::CreateRTVHeap()
	{
		Rendering::DescriptorHeap* heap = new Rendering::DescriptorHeap();
		heap->Init(m_pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_numFrames * 2);
		return heap;
	}

	Rendering::DescriptorHeap* RenderModule::CreateDSVHeap()
	{
		Rendering::DescriptorHeap* heap = new Rendering::DescriptorHeap();
		heap->Init(m_pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 2);
		return heap;
	}

	Rendering::TextureId RenderModule::GetRenderTargetTextureId(const Rendering::RenderTarget* pRenderTarget) const
	{
		return pRenderTarget->m_textureId;
	}

	LinearConstantBufferPool* RenderModule::GetLinearCBufferPool()
	{
		return m_pLinearCBufferPool;
	}

	CommandQueue* RenderModule::GetRenderCommandQueue()
	{
		return m_pRenderCommandQueue;
	}

	CommandQueue* RenderModule::GetCopyCommandQueue()
	{
		return m_pCopyCommandQueue;
	}

	ID3D12GraphicsCommandList2* RenderModule::GetRenderCommandList()
	{
		return m_pRenderCommandList;
	}

	ID3D12Device2* RenderModule::GetDevice()
	{
		return m_pDevice;
	}

	void RenderModule::ReportLiveObject()
	{
#if defined(_DEBUG)
		IDXGIDebug1* pDxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDxgiDebug))))
		{
			pDxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));

			pDxgiDebug->Release();
		}
#endif
	}

	void RenderModule::CreateDevice(IDXGIAdapter4* pAdapter)
	{
		ID3D12Device2* pD3d12Device2;
		HRESULT res = D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pD3d12Device2));
		ThrowIfFailed(res);

#if defined(_DEBUG)
		ID3D12InfoQueue* pInfoQueue;
		res = pD3d12Device2->QueryInterface(&pInfoQueue);
		ThrowIfFailed(res);

		//pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		//pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		//pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

		pInfoQueue->Release();
#endif

		m_pDevice = pD3d12Device2;
	}

	IDXGIAdapter4* RenderModule::GetAdapter(bool useWarp)
	{
		IDXGIFactory4* pDxgiFactory;
		UINT createFactoryFlag = 0;

#if defined(_DEBUG)
		createFactoryFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif

		HRESULT res = CreateDXGIFactory2(createFactoryFlag, IID_PPV_ARGS(&pDxgiFactory));
		ThrowIfFailed(res);

		IDXGIAdapter1* pDxgiAdapter1;
		IDXGIAdapter4* pDxgiAdapter4 = nullptr;

		if (useWarp)
		{
			res = pDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pDxgiAdapter1));
			ThrowIfFailed(res);
			res = pDxgiAdapter1->QueryInterface(&pDxgiAdapter4);
			ThrowIfFailed(res);

			pDxgiAdapter1->Release();
		}
		else
		{
			SIZE_T maxDedicatedVideoMemory = 0;
			for (UINT i = 0; pDxgiFactory->EnumAdapters1(i, &pDxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
				pDxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

				//check if we can create a d3d12 device and pick the adapter with most memory

				if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) //no hardware
				{
					pDxgiAdapter1->Release();
					continue;
				}

				if (dxgiAdapterDesc1.DedicatedVideoMemory <= maxDedicatedVideoMemory) //less memory
				{
					pDxgiAdapter1->Release();
					continue;
				}

				res = D3D12CreateDevice(pDxgiAdapter1, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(res))
				{
					if (pDxgiAdapter4 != nullptr)
						pDxgiAdapter4->Release();

					maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
					res = pDxgiAdapter1->QueryInterface(&pDxgiAdapter4);
					ThrowIfFailed(res);
				}

				pDxgiAdapter1->Release();

			}
		}

		pDxgiFactory->Release();

		return pDxgiAdapter4;
	}

	void RenderModule::EnableDebugLayer()
	{
#if defined(_DEBUG)
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_pDebugInterface)));
		m_pDebugInterface->EnableDebugLayer();
#endif
	}

	void RenderModule::CheckTearingSupport()
	{
		m_allowTearing = true;

		// Rather than create the DXGI 1.5 factory interface directly, we create the
		// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
		// graphics debugging tools which will not support the 1.5 factory interface 
		// until a future update.
		IDXGIFactory4* pFactory4;
		HRESULT res = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory4));
		if (SUCCEEDED(res))
		{
			IDXGIFactory5* pFactory5;
			if (SUCCEEDED(pFactory4->QueryInterface(&pFactory5)))
			{
				res = pFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &m_allowTearing, sizeof(m_allowTearing));
				if (FAILED(res))
				{
					m_allowTearing = false;
				}

				pFactory5->Release();
			}

			pFactory4->Release();
		}
	}

	void RenderModule::CreateSwapChain(HWND hWnd, ID3D12CommandQueue* pCommandQueue, uint32_t width, uint32_t height, uint32_t bufferCount)
	{
		IDXGIFactory4* pDxgiFactory4;
		UINT createFactoryFlags = 0;
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		HRESULT res = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&pDxgiFactory4));
		ThrowIfFailed(res);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = bufferCount;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		// It is recommended to always allow tearing if tearing support is available.
		swapChainDesc.Flags = m_allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		IDXGISwapChain1* pSwapChain1;
		res = pDxgiFactory4->CreateSwapChainForHwnd(pCommandQueue, hWnd, &swapChainDesc, nullptr, nullptr, &pSwapChain1);
		ThrowIfFailed(res);

		// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
		// will be handled manually.
		ThrowIfFailed(pDxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

		res = pSwapChain1->QueryInterface(&m_pSwapChain);
		ThrowIfFailed(res);

		m_mainScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
		m_mainViewport = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(width), static_cast<float>(height));

		pSwapChain1->Release();
		pDxgiFactory4->Release();
	}

	void RenderModule::UpdateRenderTargetViews()
	{
		for (int ii = 0; ii < m_numFrames; ++ii)
		{
			ID3D12Resource* backBuffer;
			ThrowIfFailed(m_pSwapChain->GetBuffer(ii, IID_PPV_ARGS(&backBuffer)));

			m_pDevice->CreateRenderTargetView(backBuffer, nullptr, m_mainRTV[ii]);

			m_pBackBuffers[ii] = backBuffer;
		}
	}

	void RenderModule::RenderBaseShape(const Mesh* pMesh, const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color) const
	{
		m_pRenderCommandList->SetPipelineState(m_pBaseShapeMaterial->m_pPipelineState->GetPipelineState());
		m_pRenderCommandList->SetGraphicsRootSignature(m_pBaseShapeMaterial->m_pRootSignature->GetRootSignature());

		DirectX::XMMATRIX wvp = txWs * m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix();
		m_pRenderCommandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &wvp, 0);
		m_pRenderCommandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMFLOAT4) / 4, &color, sizeof(DirectX::XMMATRIX) / 4);

		m_pRenderCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pRenderCommandList->IASetVertexBuffers(0, 1, &pMesh->GetVertexBufferView());
		m_pRenderCommandList->IASetIndexBuffer(&pMesh->GetIndexBufferView());

		m_pRenderCommandList->DrawIndexedInstanced(pMesh->GetIndicesCount(), 1, 0, 0, 0);
	}
}
