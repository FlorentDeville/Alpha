/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Button.h"

#include "Rendering/RenderModule.h"
#include "Rendering/Renderable/RenderableMgr.h"

#include "Widgets/Message.h"
#include "Widgets/WidgetMgr.h"

extern RenderModule* g_pRenderModule;
extern WidgetMgr* g_pWidgetMgr;

Button::Button(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: Widget(w, h, x, y)
{}

Button::~Button()
{}

void Button::Draw()
{
	DirectX::XMMATRIX mvpMatrix;
	ComputeWVPMatrix(mvpMatrix);

	DirectX::XMVECTOR color = m_backgroundColor;
	if(m_hover)
		color = DirectX::XMVectorSet(0.24f, 0.24f, 0.24f, 1.f);

	Renderable* pRenderable = g_pRenderableMgr->GetRenderable(g_pWidgetMgr->m_widgetRenderableId);
	g_pRenderModule->PreRenderForRenderable(*pRenderable);

	g_pRenderModule->SetConstantBuffer(0, sizeof(DirectX::XMMATRIX), &mvpMatrix, 0);
	g_pRenderModule->SetConstantBuffer(1, sizeof(color), &color, 0);

	int value = m_showBorder ? 1 : 0;
	g_pRenderModule->SetConstantBuffer(2, sizeof(value), &value, 0);
	g_pRenderModule->SetConstantBuffer(3, sizeof(m_borderColor), &m_borderColor, 0);

	float rect[2] = { (float)m_size.x, (float)m_size.y };
	g_pRenderModule->SetConstantBuffer(4, sizeof(rect), &rect, 0);

	g_pRenderModule->SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

	g_pRenderModule->PostRenderForRenderable(*pRenderable);

	Widget::Draw();
}

bool Button::Handle(const Message& msg)
{
	switch (msg.m_id)
	{
	case M_MouseEnter:
	{
		m_hover = true;
		m_showBorder = true;
		return true;
	}
	break;

	case M_MouseExit:
	{
		m_hover = false;
		m_showBorder = false;
		return true;
	}
	break;

	case M_MouseLDown:
	{
		if (m_onClick)
			return m_onClick();

		return false;
	}
	break;
	}

	return false;
}

void Button::OnClick(const std::function<bool()>& callback)
{
	m_onClick = callback;
}
