/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Button.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"

#include "Widgets/Events/BaseEvent.h"
#include "Widgets/Label.h"
#include "Widgets/Message.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Button::Button(uint32_t w, uint32_t h, int32_t x, int32_t y)
		: AbstractClickable(w, h, x , y)
		, m_isSelected(false)
		, m_defaultStyle()
		, m_hoverStyle()
		, m_selectedStyle()
	{
		m_hoverStyle.ShowBorder(true);
		m_hoverStyle.SetBackgroundColor(Color(0.24f, 0.24f, 0.24f, 1.f));

		m_selectedStyle.SetBackgroundColor(Color(0.24f, 0.24f, 0.24f, 1.f));
	}

	Button::Button(const std::string& textLabel)
		: Button(0, 0, 0, 0)
	{
		const int HMARGIN = 10;
		const int VMARGIN = 3;
		Label* pLabel = new Label(textLabel);
		pLabel->SetX(HMARGIN);
		pLabel->SetY(VMARGIN);

		Core::UInt2 size = pLabel->GetSize();
		size.x += (2 * HMARGIN);
		size.y += (2 * VMARGIN);
		SetSize(size);
		AddWidget(pLabel);
	}

	Button::Button(const std::string& textLabel, uint32_t width, uint32_t height)
		: Button(width, height, 0, 0)
	{
		Label* pLabel = new Label(textLabel);

		Core::UInt2 labelSize = pLabel->GetSize();

		int labelX = static_cast<int>((width - labelSize.x) * 0.5f);
		int labelY = static_cast<int>((height - labelSize.y) * 0.5f);
		
		pLabel->SetX(labelX);
		pLabel->SetY(labelY);

		AddWidget(pLabel);
	}

	Button::~Button()
	{}

	void Button::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		ButtonStyle* pCurrentStyle = &m_defaultStyle;
		if (m_hover)
			pCurrentStyle = &m_hoverStyle;
		if (m_isSelected)
			pCurrentStyle = &m_selectedStyle;

		DirectX::XMMATRIX mvpMatrix;
		ComputeWVPMatrix(windowSize, mvpMatrix);

		WidgetMgr& widgetMgr = WidgetMgr::Get();
		Rendering::RenderModule& render = Rendering::RenderModule::Get();
		Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

		const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_materialId);
		render.BindMaterial(*pMaterial, mvpMatrix);

		render.SetConstantBuffer(1, sizeof(pCurrentStyle->m_backgroundColor), &(pCurrentStyle->m_backgroundColor), 0);

		int value = pCurrentStyle->m_showBorder ? 1 : 0;
		render.SetConstantBuffer(2, sizeof(value), &value, 0);
		render.SetConstantBuffer(3, sizeof(pCurrentStyle->m_borderColor), &pCurrentStyle->m_borderColor, 0);

		float rect[2] = { (float)m_size.x, (float)m_size.y };
		render.SetConstantBuffer(4, sizeof(rect), &rect, 0);

		render.SetConstantBuffer(5, sizeof(pCurrentStyle->m_borderSize), &pCurrentStyle->m_borderSize, 0);

		const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
		render.RenderMesh(*pMesh);

		Widget::Draw(windowSize, scissor);
	}

	bool Button::Handle(const BaseEvent& ev)
	{
		switch (ev.m_id)
		{
		case EventType::kMouseEnter:
		{
			m_hover = true;
			m_showBorder = true;
			return ParentClass::Handle(ev);
		}
		break;

		case EventType::kMouseExit:
		{
			m_hover = false;
			m_showBorder = false;
			return ParentClass::Handle(ev);
		}
		break;

		case EventType::kMouseMove:
		{
			m_hover = true;
			m_showBorder = true;
			return ParentClass::Handle(ev);
		}
		break;

		default:
			return ParentClass::Handle(ev);
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

	ButtonStyle& Button::GetHoverStyle()
	{
		return m_hoverStyle;
	}

	bool Button::IsSelected() const
	{
		return m_isSelected;
	}
}
