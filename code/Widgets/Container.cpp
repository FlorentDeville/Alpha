/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Container.h"

#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"

#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Container::Container()
		: Widget()
		, m_defaultStyle()
	{
		SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
	}

	Container::Container(int width, int height)
		: Widget(width, height, 0, 0)
		, m_defaultStyle()
	{}

	Container::~Container()
	{}

	void Container::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		DirectX::XMMATRIX mvpMatrix;
		ComputeWVPMatrix(windowSize, mvpMatrix);

		WidgetMgr& widgetMgr = WidgetMgr::Get();
		Rendering::RenderModule& render = Rendering::RenderModule::Get();
		Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

		{
			D3D12_RECT rect;
			rect.left = m_absPos.x;
			rect.right = m_absPos.x + m_size.x;
			rect.top = m_absPos.y;
			rect.bottom = m_absPos.y + m_size.y;
			render.SetScissorRectangle(rect);
		}

		const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_materialId);
		render.BindMaterial(*pMaterial, mvpMatrix);

		render.SetConstantBuffer(1, sizeof(m_defaultStyle.m_backgroundColor), &m_defaultStyle.m_backgroundColor, 0);

		int valueShowBorder = m_defaultStyle.m_showBorder ? 1 : 0;
		render.SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
		render.SetConstantBuffer(3, sizeof(m_defaultStyle.m_borderColor), &m_defaultStyle.m_borderColor, 0);

		float rect[2] = { (float)m_size.x, (float)m_size.y };
		render.SetConstantBuffer(4, sizeof(rect), &rect, 0);

		render.SetConstantBuffer(5, sizeof(m_defaultStyle.m_borderSize), &m_defaultStyle.m_borderSize, 0);

		const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
		render.RenderMesh(*pMesh);

		Widget::Draw(windowSize);

		{
			D3D12_RECT windowRect;
			windowRect.left = 0;
			windowRect.right = (LONG)windowSize.x;
			windowRect.top = 0;
			windowRect.bottom = (LONG)windowSize.y;
			render.SetScissorRectangle(windowRect);
		}
	}

	void Container::ResizeChildren()
	{
		if (!m_defaultStyle.m_showBorder)
		{
			Widgets::Widget::ResizeChildren();
			return;
		}

		DirectX::XMINT3 absPos = m_absPos;
		absPos.x += m_defaultStyle.m_borderSize;
		absPos.y += m_defaultStyle.m_borderSize;

		DirectX::XMUINT2 size = m_size;
		size.x -= 2 * m_defaultStyle.m_borderSize;
		size.y -= 2 * m_defaultStyle.m_borderSize;

		for (Widget* pChild : m_children)
			pChild->Resize(absPos, size);
	}

	ContainerStyle& Container::GetDefaultStyle()
	{
		return m_defaultStyle;
	}
}
