/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/RenderTargets/RenderTarget.h"

#include "Core/Math/Vec4f.h"
#include "Core/Helper.h"

#include "Rendering/CommandQueue.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureMgr.h"
#include "Rendering/RenderModule.h"

#include "d3dx12.h"

//#pragma optimize("", off)

namespace Rendering
{
	RenderTarget::RenderTarget(int width, int height)
		: RenderTarget(width, height, DXGI_FORMAT_R8G8B8A8_UNORM)
	{ }

	RenderTarget::RenderTarget(int width, int height, DXGI_FORMAT format)
		: RenderTarget(width, height, format, Core::Vec4f(0.27f, 0.27f, 0.27f, 1.f))
	{ }

	RenderTarget::RenderTarget(int width, int height, DXGI_FORMAT format, const Core::Vec4f& clearColor)
		: m_textureId()
		, m_texture()
		, m_rtv()
		, m_dsv()
	{
		m_clearColor[0] = clearColor.GetX();
		m_clearColor[1] = clearColor.GetY();
		m_clearColor[2] = clearColor.GetZ();
		m_clearColor[3] = clearColor.GetW();

		RenderModule& renderModule = RenderModule::Get();

		//create the rtv and dsv heap
		m_pRTVHeap = renderModule.CreateRTVHeap();
		m_pDSVHeap = renderModule.CreateDSVHeap();

		Rendering::TextureMgr& textureMgr = Rendering::TextureMgr::Get();
		ID3D12Device2* pDevice = renderModule.GetDevice();

		//Create render texture and rtv
		textureMgr.CreateTexture(&m_texture, m_textureId);
		m_texture->InitAsRenderTarget(width, height, m_clearColor, format);

		m_rtv = m_pRTVHeap->GetNewHandle();
		pDevice->CreateRenderTargetView(m_texture->GetResource(), nullptr, m_rtv);

		m_scissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
		m_viewport = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(width), static_cast<float>(height));

		m_dsv = m_pDSVHeap->GetNewHandle();
		CreateDepthBuffer(width, height);
	}

	RenderTarget::~RenderTarget()
	{
		RenderModule& renderModule = RenderModule::Get();
		Rendering::TextureMgr& textureMgr = Rendering::TextureMgr::Get();
		textureMgr.DeleteTexture(m_textureId);

		m_pDepthBuffer->Release();

		m_pRTVHeap->Release();
		delete m_pRTVHeap;

		m_pDSVHeap->Release();
		delete m_pDSVHeap;
	}

	void RenderTarget::BeginScene()
	{
		m_texture->TransitionTo(D3D12_RESOURCE_STATE_RENDER_TARGET);

		RenderModule& renderModule = RenderModule::Get();
		ID3D12GraphicsCommandList2* commandList = renderModule.GetRenderCommandList();

		// Clear the render target.
		commandList->ClearRenderTargetView(m_rtv, m_clearColor, 0, nullptr);

		// Clear the depth buffer
		float depthValue = 1.f;
		commandList->ClearDepthStencilView(m_dsv, D3D12_CLEAR_FLAG_DEPTH, depthValue, 0, 0, nullptr);

		//Set viewport and scissors
		commandList->RSSetViewports(1, &m_viewport);
		commandList->RSSetScissorRects(1, &m_scissorRect);

		// Set render targets
		commandList->OMSetRenderTargets(1, &m_rtv, FALSE, &m_dsv);
	}

	void RenderTarget::EndScene()
	{
		m_texture->TransitionTo(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}

	void RenderTarget::ClearDepthBuffer()
	{
		RenderModule& renderModule = RenderModule::Get();
		ID3D12GraphicsCommandList2* commandList = renderModule.GetRenderCommandList();

		// Clear the depth buffer
		float depthValue = 1.f;
		commandList->ClearDepthStencilView(m_dsv, D3D12_CLEAR_FLAG_DEPTH, depthValue, 0, 0, nullptr);
	}

	void RenderTarget::CopyToReabackBuffer(Texture* pDestTexture)
	{
		m_texture->TransitionTo(D3D12_RESOURCE_STATE_COPY_SOURCE);

		RenderModule& renderModule = RenderModule::Get();
		ID3D12GraphicsCommandList2* pCommandList = renderModule.GetRenderCommandList();

		D3D12_TEXTURE_COPY_LOCATION dest;
		dest.pResource = pDestTexture->GetResource();
		dest.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
		renderModule.GetDevice()->GetCopyableFootprints(&m_texture->GetResourceDesc(), 0, 1, 0, &footprint, nullptr, nullptr, nullptr);

		dest.PlacedFootprint = footprint;

		const CD3DX12_TEXTURE_COPY_LOCATION copySrc(m_texture->GetResource(), 0);

		pCommandList->CopyTextureRegion(&dest, 0, 0, 0, &copySrc, nullptr);
	}

	void RenderTarget::CreateDepthBuffer(int width, int height)
	{
		RenderModule& renderModule = RenderModule::Get();
		// Flush any GPU commands that might be referencing the depth buffer.
		renderModule.GetCopyCommandQueue()->Flush();
		renderModule.GetRenderCommandQueue()->Flush();

		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		if (m_pDepthBuffer)
			m_pDepthBuffer->Release();

		ID3D12Device2* pDevice = renderModule.GetDevice();

		ThrowIfFailed(pDevice->CreateCommittedResource(
			&heapProperty,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&m_pDepthBuffer)
		));

		// Update the depth-stencil view.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		pDevice->CreateDepthStencilView(m_pDepthBuffer, &dsvDesc, m_dsv);
	}

}
