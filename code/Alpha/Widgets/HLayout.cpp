/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "HLayout.h"

#include "Rendering/RenderModule.h"
#include "Rendering/Renderable/RenderableMgr.h"
#include "Widgets/WidgetMgr.h"

extern RenderModule* g_pRenderModule;
extern WidgetMgr* g_pWidgetMgr;

HLayout::HLayout(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: Widget(w, h, x ,y)
{}

HLayout::~HLayout()
{}

void HLayout::Draw()
{
	DirectX::XMMATRIX wvp;
	ComputeWVPMatrix(wvp);
	int valueShowBorder = m_showBorder ? 1 : 0;
	float rect[2] = { (float)m_size.x, (float)m_size.y};

	{
		const Renderable* pRenderable = g_pRenderableMgr->GetRenderable(g_pWidgetMgr->m_widgetRenderableId);
		g_pRenderModule->PreRenderForRenderable(*pRenderable);

		g_pRenderModule->SetConstantBuffer(0, sizeof(DirectX::XMMATRIX), &wvp, 0);
		g_pRenderModule->SetConstantBuffer(1, sizeof(m_backgroundColor), &m_backgroundColor, 0);	
		g_pRenderModule->SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
		g_pRenderModule->SetConstantBuffer(3, sizeof(m_borderColor), &m_borderColor, 0);	
		g_pRenderModule->SetConstantBuffer(4, sizeof(rect), &rect, 0);
		g_pRenderModule->SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

		g_pRenderModule->PostRenderForRenderable(*pRenderable);
	}

	for (Widget* pWidget : m_children)
		pWidget->Draw();
}

void HLayout::Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
{
	m_absPos.x = parentAbsPos.x + m_locPos.x;
	m_absPos.y = parentAbsPos.y + m_locPos.y;
	m_absPos.z = parentAbsPos.z + m_locPos.z - 1;
	
	int32_t x = 0;
	if (!m_children.empty())
		x = m_children.front()->GetX();

	for(Widget* pWidget : m_children)
	{
		pWidget->SetX(x);
		x = pWidget->GetX() + pWidget->GetWidth();
	}

	Widget::Resize(parentAbsPos, parentSize);
}
