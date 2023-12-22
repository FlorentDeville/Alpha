/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Viewport_v2.h"

#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RenderTargets/RenderTarget.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Texture/Texture.h"
#include "Rendering/Texture/TextureId.h"
#include "Rendering/Texture/TextureMgr.h"

#include "Widgets/Message.h"
#include "Widgets/WidgetMgr.h"

#include <DirectXMath.h>
using namespace DirectX;

namespace Widgets
{
	Viewport_v2::Viewport_v2(int width, int height)
		: Widget()
		, m_width(width)
		, m_height(height)
	{
		m_pRenderTarget = Rendering::RenderModule::Get().CreateRenderTarget(width, height);
		m_aspectRatio = m_width / static_cast<float>(m_height);
	}

	Viewport_v2::~Viewport_v2()
	{
		delete m_pRenderTarget;
	}

	void Viewport_v2::Render()
	{}

	void Viewport_v2::ClearDepthBuffer()
	{
		m_pRenderTarget->ClearDepthBuffer();
	}

	void Viewport_v2::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		WidgetMgr& widgetMgr = WidgetMgr::Get();

		const Rendering::PipelineState* pPipelineState = Rendering::PipelineStateMgr::Get().GetPipelineState(WidgetMgr::Get().m_widgetViewportPsoId);
		const Rendering::RootSignature* pRootSignature = Rendering::RootSignatureMgr::Get().GetRootSignature(pPipelineState->GetRootSignatureId());
		const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
		
		Rendering::TextureId renderTargetTextureId = renderModule.GetRenderTargetTextureId(m_pRenderTarget);

		Rendering::Texture* pTexture = Rendering::TextureMgr::Get().GetTexture(renderTargetTextureId);
		ID3D12DescriptorHeap* pSrv = pTexture->GetSRV();

		ID3D12GraphicsCommandList2* pCommandList = Rendering::RenderModule::Get().GetRenderCommandList();

		pCommandList->SetPipelineState(pPipelineState->GetPipelineState());
		pCommandList->SetGraphicsRootSignature(pRootSignature->GetRootSignature());

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pCommandList->IASetVertexBuffers(0, 1, &pMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&pMesh->GetIndexBufferView());

		//Set Constant buffer
		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(windowSize, wvp);
		pCommandList->SetGraphicsRoot32BitConstants(0, sizeof(wvp) / 4, &wvp, 0);

		pCommandList->SetGraphicsRoot32BitConstants(1, sizeof(m_size) / 4, &m_size, 0);

		DirectX::XMFLOAT2 textureRect;
		textureRect.x = (float)pTexture->GetWidth();
		textureRect.y = (float)pTexture->GetHeight();
		pCommandList->SetGraphicsRoot32BitConstants(2, sizeof(textureRect) / 4, &textureRect, 0);

		//Set texture srv
		ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
		pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
		pCommandList->SetGraphicsRootDescriptorTable(3, pSrv->GetGPUDescriptorHandleForHeapStart());

		//Draw!!!
		pCommandList->DrawIndexedInstanced(pMesh->GetIndicesCount(), 1, 0, 0, 0);
	}

	void Viewport_v2::Internal_Render()
	{
		m_pRenderTarget->BeginScene();
		Render();
		m_pRenderTarget->EndScene();
	}
}
