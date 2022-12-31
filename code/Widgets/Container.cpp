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

		//DirectX::XMVECTOR color = m_backgroundColor;

		WidgetMgr& widgetMgr = WidgetMgr::Get();
		RenderModule& render = RenderModule::Get();
		Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

		const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_materialId);
		render.BindMaterial(*pMaterial, mvpMatrix);

		render.SetConstantBuffer(1, sizeof(m_defaultStyle.m_backgroundColor), &m_defaultStyle.m_backgroundColor, 0);

		int valueShowBorder = m_defaultStyle.m_showBorder ? 1 : 0;
		render.SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
		render.SetConstantBuffer(3, sizeof(m_defaultStyle.m_borderColor), &m_defaultStyle.m_borderColor, 0);

		float rect[2] = { (float)m_size.x, (float)m_size.y };
		render.SetConstantBuffer(4, sizeof(rect), &rect, 0);

		//m_borderWidth = 3;
		render.SetConstantBuffer(5, sizeof(m_defaultStyle.m_borderSize), &m_defaultStyle.m_borderSize, 0);

		const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
		render.RenderMesh(*pMesh);

		Widget::Draw(windowSize);
	}

	ContainerStyle& Container::GetDefaultStyle()
	{
		return m_defaultStyle;
	}
}
