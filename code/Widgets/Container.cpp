/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Container.h"

#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"
#include "Rendering/RenderModule.h"

#include "Widgets/Tools/AlternateColorCBuffer.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Container::Container()
		: Widget()
		, m_defaultStyle()
		, m_hoverStyle()
		, m_currentState(State::Default)
	{
		SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
	}

	Container::Container(int width, int height)
		: Widget(width, height, 0, 0)
		, m_defaultStyle()
		, m_hoverStyle()
		, m_currentState(State::Default)
	{}

	Container::~Container()
	{}

	void Container::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		ContainerStyle* pStyle = &m_defaultStyle;
		if (m_currentState == Hover)
			pStyle = &m_hoverStyle;

		DirectX::XMMATRIX mvpMatrix;
		ComputeWVPMatrix(windowSize, mvpMatrix);

		WidgetMgr& widgetMgr = WidgetMgr::Get();
		Rendering::RenderModule& render = Rendering::RenderModule::Get();
		Rendering::PipelineStateMgr& psoMgr = Rendering::PipelineStateMgr::Get();
		
		D3D12_RECT localScissorRect;
		localScissorRect.left = m_absPos.x;
		localScissorRect.right = m_absPos.x + m_size.x;
		localScissorRect.top = m_absPos.y;
		localScissorRect.bottom = m_absPos.y + m_size.y;

		//intersection with the parent scissor
		if (localScissorRect.left < scissor.left) localScissorRect.left = scissor.left;
		if (localScissorRect.right > scissor.right) localScissorRect.right = scissor.right;
		if (localScissorRect.top < scissor.top) localScissorRect.top = scissor.top;
		if (localScissorRect.bottom > scissor.bottom) localScissorRect.bottom = scissor.bottom;

		if (localScissorRect.left >= localScissorRect.right || localScissorRect.top >= localScissorRect.bottom)
			return;

		if (pStyle->m_backgroundColor.m_channels[3] != 0)
		{
			render.SetScissorRectangle(localScissorRect);

			const Rendering::PipelineState* pPso = psoMgr.GetPipelineState(widgetMgr.GetBaseWidgetPsoId());
			render.BindMaterial(*pPso);

			render.SetConstantBuffer(0, sizeof(mvpMatrix), &mvpMatrix, 0);
			render.SetConstantBuffer(1, sizeof(pStyle->m_backgroundColor), &pStyle->m_backgroundColor, 0);

			int valueShowBorder = pStyle->m_showBorder ? 1 : 0;
			render.SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
			render.SetConstantBuffer(3, sizeof(pStyle->m_borderColor), &pStyle->m_borderColor, 0);

			float rect[2] = { (float)m_size.x, (float)m_size.y };
			render.SetConstantBuffer(4, sizeof(rect), &rect, 0);

			render.SetConstantBuffer(5, sizeof(pStyle->m_borderSize), &pStyle->m_borderSize, 0);

			//containers don't use alternate color but if the constant buffer is not set, it messes up the shader execution.
			AlternateColorCBuffer altColor;
			altColor.m_size = 0;
			render.SetConstantBuffer(6, sizeof(AlternateColorCBuffer), &altColor, 0);

			const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
			render.RenderMesh(*pMesh);
		}

		Widget::Draw(windowSize, localScissorRect);

		//reset the scissor
		render.SetScissorRectangle(scissor);
	}

	bool Container::Handle(const BaseEvent& ev)
	{
		switch (ev.m_id)
		{
		case EventType::kMouseEnter:
		{
			OutputDebugString("Enter\n");
			if (m_currentState == Default)
				m_currentState = Hover;
			return true;
		}
		break;

		case EventType::kMouseExit:
		{
			OutputDebugString("Exit\n");
			if (m_currentState == Hover)
				m_currentState = Default;
			return true;
		}
		break;

		default:
			return Widget::Handle(ev);
			break;
		}

		return false;
	}

	void Container::ResizeChildren()
	{
		if (!m_defaultStyle.m_showBorder)
		{
			Widgets::Widget::ResizeChildren();
			return;
		}

		Core::Int3 absPos = m_absPos;
		absPos.x += m_defaultStyle.m_borderSize;
		absPos.y += m_defaultStyle.m_borderSize;

		Core::UInt2 size = m_size;
		size.x -= 2 * m_defaultStyle.m_borderSize;
		size.y -= 2 * m_defaultStyle.m_borderSize;

		for (Widget* pChild : m_children)
			pChild->Resize(absPos, size);
	}

	ContainerStyle& Container::GetDefaultStyle()
	{
		return m_defaultStyle;
	}

	ContainerStyle& Container::GetHoverStyle()
	{
		return m_hoverStyle;
	}
}
