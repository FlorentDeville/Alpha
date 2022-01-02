/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Viewport.h"

#include "Rendering/Mesh/Mesh.h"
#include "Rendering/Renderable/RenderableMgr.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Widgets/WidgetMgr.h"

extern RenderModule* g_pRenderModule;
extern PipelineStateId g_widgetViewportPsoId;
extern TextureId g_textureId;

namespace Widgets
{
	Viewport::Viewport()
		:Widget()
	{}

	Viewport::~Viewport()
	{}

	void Viewport::Draw()
	{
		const Renderable* pRenderable = g_pRenderableMgr->GetRenderable(g_pWidgetMgr->m_widgetRenderableId);
		const PipelineState* pPipelineState = g_pPipelineStateMgr->GetResource(g_widgetViewportPsoId);
		RootSignature* pRootSignature = g_pRootSignatureMgr->GetRootSignature(pPipelineState->GetRootSignatureId());
		const Mesh* pMesh = g_pMeshMgr->GetMesh(pRenderable->GetMeshId());
		Texture* pTexture = g_pTextureMgr->GetResource(g_pRenderModule->GetRenderTextureId());
		ID3D12DescriptorHeap* pSrv = pTexture->GetSRV();

		ID3D12GraphicsCommandList2* pCommandList = g_pRenderModule->GetRenderCommandList();

		pCommandList->SetPipelineState(pPipelineState->GetPipelineState());
		pCommandList->SetGraphicsRootSignature(pRootSignature->GetRootSignature());

		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pCommandList->IASetVertexBuffers(0, 1, &pMesh->GetVertexBufferView());
		pCommandList->IASetIndexBuffer(&pMesh->GetIndexBufferView());

		//Set Constant buffer
		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(wvp);
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
}
