/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Layout.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Message.h"

namespace Widgets
{
	Layout::Layout(uint32_t w, uint32_t h, int32_t x, int32_t y)
		: Widget(w, h, x, y)
		, m_space(0, 0)
		, m_previousMousePosition(0, 0)
		, m_dir(Layout::Direction::Horizontal)
	{}

	Layout::~Layout()
	{}

	void Layout::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(windowSize, wvp);
		int valueShowBorder = m_showBorder ? 1 : 0;
		float rect[2] = { (float)m_size.x, (float)m_size.y };

		{
			WidgetMgr& widgetMgr = WidgetMgr::Get();
			RenderModule& render = RenderModule::Get();
			Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

			const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_materialId);
			render.BindMaterial(*pMaterial, wvp);

			render.SetConstantBuffer(1, sizeof(m_backgroundColor), &m_backgroundColor, 0);
			render.SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
			render.SetConstantBuffer(3, sizeof(m_borderColor), &m_borderColor, 0);
			render.SetConstantBuffer(4, sizeof(rect), &rect, 0);
			render.SetConstantBuffer(5, sizeof(m_borderWidth), &m_borderWidth, 0);

			const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
			render.RenderMesh(*pMesh);
		}

		for (Widget* pWidget : m_children)
			pWidget->Draw(windowSize);
	}

	bool Layout::Handle(const Message& msg)
	{
		switch (msg.m_id)
		{
		case M_MouseMove:
			if (m_onMouseMove)
				return m_onMouseMove(msg.m_low.m_pos[0], msg.m_low.m_pos[1], (MouseKey)msg.m_high);
			break;

		default:
			return Widget::Handle(msg);
			break;
		}

		return false;
	}

	void Layout::Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		m_absPos.x = parentAbsPos.x + m_locPos.x;
		m_absPos.y = parentAbsPos.y + m_locPos.y;
		m_absPos.z = parentAbsPos.z + m_locPos.z - 1;

		ReComputeSize(parentSize);

		if (m_children.empty())
			return;

		int32_t pos = 0;
		switch (m_dir)
		{
		case Horizontal:
			pos = m_children.front()->GetX();
			break;

		case Horizontal_Reverse:
			pos = m_size.x;
			break;

		case Vertical:
			pos = m_children.front()->GetY();
			break;

		default:
			assert(false);
		}

		for (Widget* pWidget : m_children)
		{
			switch (m_dir)
			{
			case Horizontal:
				pWidget->SetX(pos);
				pos = pWidget->GetX() + pWidget->GetWidth() + m_space.x;
				break;

			case Horizontal_Reverse:
				pos -= pWidget->GetWidth() - m_space.x;
				pWidget->SetX(pos);
				break;

			case Vertical:
				pWidget->SetY(pos);
				pos = pWidget->GetY() + pWidget->GetHeight() + m_space.y;
				break;

			default:
				assert(false);
				break;
			}
		}

		Widget::Resize(parentAbsPos, parentSize);
	}

	void Layout::SetDirection(Direction dir)
	{
		m_dir = dir;
	}

	void Layout::SetSpace(const DirectX::XMINT2& space)
	{
		m_space = space;
	}
}
