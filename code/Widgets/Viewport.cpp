/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Viewport.h"

#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Widgets/Message.h"
#include "Widgets/WidgetMgr.h"

#include <DirectXMath.h>
using namespace DirectX;

extern TextureId g_textureId;

extern DirectX::XMVECTOR g_eyePosition;
extern DirectX::XMVECTOR g_euler;
extern DirectX::XMVECTOR g_direction;

namespace Widgets
{
	Viewport::Viewport()
		:Widget()
	{}

	Viewport::~Viewport()
	{}

	void Viewport::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		WidgetMgr& widgetMgr = WidgetMgr::Get();
		const PipelineState* pPipelineState = g_pPipelineStateMgr->GetResource(WidgetMgr::Get().m_widgetViewportPsoId);
		RootSignature* pRootSignature = g_pRootSignatureMgr->GetRootSignature(pPipelineState->GetRootSignatureId());
		const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
		Texture* pTexture = RenderModule::Get().GetTextureMgr().GetResource(m_onGetRenderTargetTexture());
		ID3D12DescriptorHeap* pSrv = pTexture->GetSRV();

		ID3D12GraphicsCommandList2* pCommandList = RenderModule::Get().GetRenderCommandList();

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

	void Viewport::OnGetRenderTargetTexture(const std::function<TextureId ()>& callback)
	{
		m_onGetRenderTargetTexture = callback;
	}
}
