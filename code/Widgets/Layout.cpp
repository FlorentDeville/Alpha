/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Layout.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"

#include "Widgets/Events/BaseEvent.h"
#include "Widgets/Message.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Layout::Layout()
		: Widget()
		, m_space(0, 0)
		, m_previousMousePosition(0, 0)
		, m_dir(Layout::Direction::Horizontal)
		, m_defaultStyle()
		, m_hoverStyle()
		, m_transparent(false)
	{}

	Layout::Layout(uint32_t w, uint32_t h, int32_t x, int32_t y)
		: Widget(w, h, x, y)
		, m_space(0, 0)
		, m_previousMousePosition(0, 0)
		, m_dir(Layout::Direction::Horizontal)
		, m_defaultStyle()
		, m_transparent(false)
	{}

	Layout::Layout(Direction dir, Widget::SIZE_STYLE sizeStyle)
		: Layout()
	{
		m_dir = dir;
		SetSizeStyle(sizeStyle);
	}

	Layout::~Layout()
	{}

	void Layout::Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor)
	{
		if (!m_transparent)
		{
			LayoutStyle* pCurrentStyle = &m_defaultStyle;
			if (m_hover)
				pCurrentStyle = &m_hoverStyle;

			DirectX::XMMATRIX wvp;
			ComputeWVPMatrix(windowSize, wvp);
			int valueShowBorder = pCurrentStyle->m_showBorder ? 1 : 0;
			float rect[2] = { (float)m_size.x, (float)m_size.y };

			{
				WidgetMgr& widgetMgr = WidgetMgr::Get();
				Rendering::RenderModule& render = Rendering::RenderModule::Get();
				Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

				render.SetScissorRectangle(scissor);

				const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_materialId);
				render.BindMaterial(*pMaterial, wvp);

				render.SetConstantBuffer(1, sizeof(pCurrentStyle->m_backgroundColor), &pCurrentStyle->m_backgroundColor, 0);
				render.SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
				render.SetConstantBuffer(3, sizeof(pCurrentStyle->m_borderColor), &pCurrentStyle->m_borderColor, 0);
				render.SetConstantBuffer(4, sizeof(rect), &rect, 0);
				render.SetConstantBuffer(5, sizeof(pCurrentStyle->m_borderSize), &pCurrentStyle->m_borderSize, 0);

				const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
				render.RenderMesh(*pMesh);
			}
		}

		for (Widget* pWidget : m_children)
		{
			if (!pWidget->IsEnabled())
				continue;

			pWidget->Draw(windowSize, scissor);
		}
	}

	bool Layout::Handle(const BaseEvent& ev)
	{
		switch (ev.m_id)
		{
		case EventType::kMouseEnter:
		{
			m_hover = true;
			return true;
		}
		break;

		case EventType::kMouseExit:
		{
			m_hover = false;
			return true;
		}
		break;

		default:
			return Widget::Handle(ev);
			break;
		}

		return false;
	}

	void Layout::Resize(const Core::Int3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		m_absPos.x = parentAbsPos.x + m_locPos.x;
		m_absPos.y = parentAbsPos.y + m_locPos.y;
		m_absPos.z = parentAbsPos.z + m_locPos.z - 1;

		ReComputeSize(parentSize);
		ReComputePosition(parentAbsPos, parentSize);

		if (m_children.empty())
			return;
		
		DirectX::XMUINT2 contentSize = m_size;
		int32_t pos = 0;
		if (m_defaultStyle.m_showBorder)
		{
			pos = m_defaultStyle.m_borderSize;

			contentSize.x -= 2 * m_defaultStyle.m_borderSize;
			contentSize.y -= 2 * m_defaultStyle.m_borderSize;
		}
		
		for (Widget* pWidget : m_children)
		{
			if (!pWidget->IsEnabled())
				continue;

			switch (m_dir)
			{
			case Horizontal:
			case Horizontal_Reverse:
				//compute first the new position
				pWidget->SetX(pos);

				if (m_defaultStyle.m_showBorder)
					pWidget->SetY(m_defaultStyle.m_borderSize);

				//now compute the size
				pWidget->Resize(m_absPos, contentSize);

				//move the position for the next widgets
				pos = pWidget->GetX() + pWidget->GetWidth() + m_space.x;
				break;

			case Vertical:
			case Vertical_Reverse:
				pWidget->SetY(pos);
				if (m_defaultStyle.m_showBorder)
					pWidget->SetX(m_defaultStyle.m_borderSize);

				pWidget->Resize(m_absPos, contentSize);
				pos = pWidget->GetY() + pWidget->GetHeight() + m_space.y;
				break;

			default:
				assert(false);
				break;
			}
		}

		// To compute correctly the horizontal reverse, I first compute the size and location as if it was horizontal. This
		// let's me compute the correct size of all the children widgets. Then I reverse the positions.
		if (m_dir == Horizontal_Reverse)
		{
			pos = contentSize.x;

			for (Widget* pWidget : m_children)
			{
				if (!pWidget->IsEnabled())
					continue;

				pos -= pWidget->GetWidth() - m_space.x;
				pWidget->SetX(pos);

				// Do not call Resize because it will change the size. 
				// I only need ReComputePosition to compute the absolute position and ResizeChildren to recursively recompute the absolute position of children.
				pWidget->ReComputePosition(m_absPos, contentSize);
				pWidget->ResizeChildren();
			}
		}
		else if (m_dir == Vertical_Reverse)
		{
			pos = contentSize.y;

			for (Widget* pWidget : m_children)
			{
				if (!pWidget->IsEnabled())
					continue;

				pos -= pWidget->GetHeight() - m_space.y;
				pWidget->SetY(pos);

				// Do not call Resize because it will change the size. 
				// I only need ReComputePosition to compute the absolute position and ResizeChildren to recursively recompute the absolute position of children.
				pWidget->ReComputePosition(m_absPos, contentSize);
				pWidget->ResizeChildren();
			}
		}

		ReComputeSize_PostChildren();	
	}

	void Layout::ReComputeSize_PostChildren()
	{
		if (m_sizeStyle & Widgets::Widget::HSIZE_FIT)
		{
			int maxSize = 0;
			for (const Widget* pChild : m_children)
			{
				if (!pChild->IsEnabled())
					continue;

				int max = pChild->GetWidth() + pChild->GetX();
				if (max > maxSize) maxSize = max;
			}

			m_size.x = maxSize;
			if (m_defaultStyle.m_showBorder)
			{
				m_size.x += m_defaultStyle.m_borderSize;
			}
		}
		if (m_sizeStyle & Widgets::Widget::VSIZE_FIT)
		{
			int maxSize = 0;
			for (const Widget* pChild : m_children)
			{
				if (!pChild->IsEnabled())
					continue;

				int max = pChild->GetHeight() + pChild->GetY();
				if (max > maxSize) maxSize = max;
			}

			m_size.y = maxSize;
			if (m_defaultStyle.m_showBorder)
			{
				m_size.y += m_defaultStyle.m_borderSize;
			}
		}

		
	}

	void Layout::SetDirection(Direction dir)
	{
		m_dir = dir;
	}

	void Layout::SetSpace(const DirectX::XMINT2& space)
	{
		m_space = space;
	}

	void Layout::SetTransparent(bool transparent)
	{
		m_transparent = transparent;
	}

	LayoutStyle& Layout::GetDefaultStyle()
	{
		return m_defaultStyle;
	}

	LayoutStyle& Layout::GetHoverStyle()
	{
		return m_hoverStyle;
	}
}
