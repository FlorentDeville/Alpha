/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/RenderTargets/RenderTarget.h"

#include "Core/Helper.h"

#include "Rendering/CommandQueue.h"
#include "Rendering/RenderModule.h"

#include "d3dx12.h"

namespace Rendering
{
	RenderTarget::RenderTarget(int numFrame, int width, int height, DescriptorHeap& rtvHeap, DescriptorHeap& dsvHeap)
		: m_numFrames(numFrame)
		, m_renderTargets(nullptr)
		, m_rtv(nullptr)
		, m_dsv()
	{
		m_renderTargetsId = new TextureId[numFrame];
		m_renderTargets = new Texture*[numFrame];
		m_rtv = new D3D12_CPU_DESCRIPTOR_HANDLE[numFrame];

		RenderModule::TextureMgr& textureMgr = RenderModule::Get().GetTextureMgr();
		ID3D12Device2* pDevice = RenderModule::Get().GetDevice();

		//Create render texture and rtv
		for (int ii = 0; ii < m_numFrames; ++ii)
		{
			m_renderTargets[ii] = textureMgr.CreateResource(m_renderTargetsId[ii], "render texture");
			m_renderTargets[ii]->Init_RenderTarget(width, height);

			m_rtv[ii] = rtvHeap.GetNewHandle();
			pDevice->CreateRenderTargetView(m_renderTargets[ii]->GetResource(), nullptr, m_rtv[ii]);
		}

		m_scissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
		m_viewport = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(width), static_cast<float>(height));

		m_dsv = dsvHeap.GetNewHandle();
		CreateDepthBuffer(width, height);
	}

	RenderTarget::~RenderTarget()
	{
		delete m_renderTargetsId;
		delete m_renderTargets;
		delete m_rtv;
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
