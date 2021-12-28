/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "HLayout.h"

#include "Rendering/RenderModule.h"
#include "Rendering/Renderable/RenderableMgr.h"

extern RenderModule* g_pRenderModule;

extern RenderableId g_SimpleQuadId;

HLayout::HLayout(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: Widget(w, h, x ,y)
{}

HLayout::~HLayout()
{}

void HLayout::Draw(int32_t parentX, int32_t parentY, float parentZ)
{
	DirectX::XMMATRIX wvp;
	ComputeWVPMatrix(wvp, parentX, parentY, parentZ);
	int valueShowBorder = m_showBorder ? 1 : 0;
	float rect[2] = { (float)m_width, (float)m_height };

	{
		const Renderable* pRenderable = g_pRenderableMgr->GetRenderable(g_SimpleQuadId);
		g_pRenderModule->PreRenderForRenderable(*pRenderable);

		g_pRenderModule->SetConstantBuffer(0, sizeof(DirectX::XMMATRIX), &wvp, 0);
		g_pRenderModule->SetConstantBuffer(1, sizeof(m_backgroundColor), &m_backgroundColor, 0);	
		g_pRenderModule->SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
		g_pRenderModule->SetConstantBuffer(3, sizeof(m_borderColor), &m_borderColor, 0);	
		g_pRenderModule->SetConstantBuffer(4, sizeof(rect), &rect, 0);
		g_pRenderModule->SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

		g_pRenderModule->PostRenderForRenderable(*pRenderable);
	}

	ComputeWorldPosition(parentX, parentY, m_wx, m_wy);
	ComputeScreenPosition(m_wx, m_wy, m_screenX, m_screenY);

	for (Widget* pWidget : m_children)
		pWidget->Draw(m_wx, m_wy, parentZ - 0.1f);
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
