/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Button.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"

#include "Widgets/Message.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Button::Button(uint32_t w, uint32_t h, int32_t x, int32_t y)
		: Widget(w, h, x, y)
		, m_isSelected(false)
	{}

	Button::~Button()
	{}

	void Button::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		DirectX::XMMATRIX mvpMatrix;
		ComputeWVPMatrix(windowSize, mvpMatrix);

		DirectX::XMVECTOR color = m_backgroundColor;
		if (m_hover || m_isSelected)
			color = DirectX::XMVectorSet(0.24f, 0.24f, 0.24f, 1.f);

		WidgetMgr& widgetMgr = WidgetMgr::Get();
		RenderModule& render = RenderModule::Get();
		Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

		const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_materialId);
		render.BindMaterial(*pMaterial, mvpMatrix);

		render.SetConstantBuffer(1, sizeof(color), &color, 0);

		int value = m_showBorder ? 1 : 0;
		render.SetConstantBuffer(2, sizeof(value), &value, 0);
		render.SetConstantBuffer(3, sizeof(m_borderColor), &m_borderColor, 0);

		float rect[2] = { (float)m_size.x, (float)m_size.y };
		render.SetConstantBuffer(4, sizeof(rect), &rect, 0);

		render.SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

		const Mesh* pMesh = g_pMeshMgr->GetMesh(widgetMgr.m_quadMeshId);
		render.RenderMesh(*pMesh);

		Widget::Draw(windowSize);
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

		default:
			return Widget::Handle(msg);
			break;
		}
	}

	void Button::Select()
	{
		m_isSelected = true;
	}

	void Button::Unselect()
	{
		m_isSelected = false;
	}
}
