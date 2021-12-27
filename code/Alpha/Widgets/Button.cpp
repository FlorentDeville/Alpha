/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Button.h"

#include "Rendering/RenderModule.h"
#include "Rendering/Renderable/RenderableMgr.h"

#include "Widgets/Message.h"

extern RenderModule* g_pRenderModule;
extern RenderableId g_SimpleQuadId;

Button::Button(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: Widget(w, h, x, y)
{}

Button::~Button()
{}

void Button::Draw(int32_t parentX, int32_t parentY, float parentZ)
{
	DirectX::XMMATRIX mvpMatrix;
	ComputeWVPMatrix(mvpMatrix, parentX, parentY, parentZ);
	ComputeWorldPosition(parentX, parentY, m_wx, m_wy);
	ComputeScreenPosition(m_wx, m_wy, m_screenX, m_screenY);

	DirectX::XMVECTOR color = DirectX::XMVectorSet(0.12f, 0.12f, 0.12f, 1.f);
	if(m_hover)
		color = DirectX::XMVectorSet(0.24f, 0.24f, 0.24f, 1.f);

	g_pRenderModule->Render(*g_pRenderableMgr->GetRenderable(g_SimpleQuadId), mvpMatrix, color);
}

bool Button::Handle(const Message& msg)
{
	switch (msg.m_id)
	{
	case M_MouseEnter:
	{
		m_hover = true;
		return true;
	}
	break;

	case M_MouseExit:
	{
		m_hover = false;
		return true;
	}
	break;
	}

	return false;
}
