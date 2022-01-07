/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Icon.h"

#include "Rendering/RenderModule.h"
#include "Rendering/Renderable/RenderableMgr.h"

#include "Widgets/WidgetMgr.h"

Icon::Icon(const DirectX::XMINT2& pos, const DirectX::XMUINT2 size, const std::string& path)
	: Widget(size.x, size.y, pos.x, pos.y)
{
	Texture* pTexture = g_pTextureMgr->CreateResource(m_textureId, path);
	pTexture->Init(path);
}

Icon::~Icon()
{}

void Icon::Draw()
{
	DirectX::XMMATRIX wvp;
	ComputeWVPMatrix(wvp);

	const Renderable* pRenderable = g_pRenderableMgr->GetRenderable(WidgetMgr::Get().m_iconRenderableId);
	g_pRenderModule->PreRenderForRenderable(*pRenderable);

	g_pRenderModule->SetConstantBuffer(0, sizeof(wvp), &wvp, 0);

	ID3D12DescriptorHeap* pSrv = g_pTextureMgr->GetResource(m_textureId)->GetSRV();
	ID3D12DescriptorHeap* pDescriptorHeap[] = { pSrv };
	g_pRenderModule->GetRenderCommandList()->SetDescriptorHeaps(_countof(pDescriptorHeap), pDescriptorHeap);
	g_pRenderModule->GetRenderCommandList()->SetGraphicsRootDescriptorTable(1, pSrv->GetGPUDescriptorHandleForHeapStart());

	g_pRenderModule->PostRenderForRenderable(*pRenderable);
}
