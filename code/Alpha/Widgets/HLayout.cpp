/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "HLayout.h"

#include "Rendering/RenderModule.h"

extern RenderModule* g_pRenderModule;
extern MeshId g_QuadMeshId;

HLayout::HLayout(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: Widget(w, h, x ,y)
{}

HLayout::~HLayout()
{}

void HLayout::Draw(int32_t parentX, int32_t parentY, float parentZ)
{
	DirectX::XMMATRIX wvp;
	ComputeWVPMatrix(wvp, parentX, parentY, parentZ);
	Renderable renderable(g_QuadMeshId, g_pRenderModule->m_base_PosColor_pipelineStateId);
	g_pRenderModule->Render(renderable, wvp);

	int32_t x = 0, y = 0;
	ComputeWorldPosition(parentX, parentY, x, y);
	for (Widget* pWidget : m_children)
		pWidget->Draw(x, y, parentZ - 0.1f);
}

void HLayout::Resize()
{
	int32_t x = 0;

	if (!m_children.empty())
		x = m_children.front()->GetX();

	for(Widget* pWidget : m_children)
	{
		pWidget->SetX(x);
		x = pWidget->GetX() + pWidget->GetWidth();
	}
}
