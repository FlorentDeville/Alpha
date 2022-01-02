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
extern PipelineStateId g_texture_posuv_pipelineStateId;
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
		const PipelineState* pPipelineState = g_pPipelineStateMgr->GetResource(g_texture_posuv_pipelineStateId);
		RootSignature* pRootSignature = g_pRootSignatureMgr->GetRootSignature(pPipelineState->GetRootSignatureId());
		const Mesh* pMesh = g_pMeshMgr->GetMesh(pRenderable->GetMeshId());
		ID3D12DescriptorHeap* pSrv = g_pTextureMgr->GetResource(g_pRenderModule->GetRenderTextureId())->GetSRV();
		//ID3D12DescriptorHeap* pSrv = g_pTextureMgr->GetResource(g_textureId)->GetSRV();

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

		//Set texture srv
		ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
		pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
		pCommandList->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());

		//Draw!!!
		pCommandList->DrawIndexedInstanced(pMesh->GetIndicesCount(), 1, 0, 0, 0);
	}
}
