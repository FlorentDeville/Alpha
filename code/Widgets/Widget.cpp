/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widget.h"

#include "Widgets/Events/BaseEvent.h"
#include "Widgets/Message.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	const float Widget::NEAR_CAMERA_PLANE = 0.1f;
	const float Widget::FAR_CAMERA_PLANE = 100;

	Widget::Widget()
		: m_children()
		, m_size(0, 0)
		, m_defaultSize(0, 0)
		, m_locPos(0, 0, 0)
		, m_absPos(0, 0, 0)
		, m_hover(false)
		, m_backgroundColor(DirectX::XMVectorSet(0.12f, 0.12f, 0.12f, 1.f))
		, m_borderColor(DirectX::XMVectorSet(1.f, 1.f, 1.f, 1.f))
		, m_showBorder(false)
		, m_borderWidth(1)
		, m_sizeStyle(HSIZE_DEFAULT | VSIZE_DEFAULT)
		, m_hPositionStyle(HPOSITION_STYLE::NONE)
		, m_vPositionStyle(VPOSITION_STYLE::NONE)
		, m_name()
		, m_enabled(true)
		, m_focusPolicy(FOCUS_POLICY::DEFAULT)
		, m_pParent(nullptr)
		, m_padding()
	{}

	Widget::Widget(uint32_t w, uint32_t h, int32_t x, int32_t y)
		: Widget()
	{
		m_size.x = w;
		m_size.y = h;
		m_defaultSize = m_size;
		m_locPos = DirectX::XMINT3(x, y, 0);
	}

	Widget::~Widget()
	{
		for (const Widget* pWidget : m_children)
			delete pWidget;
	}

	void Widget::Update(uint64_t dt)
	{}

	void Widget::Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor)
	{
		if (!IsEnabled())
			return;

		for (Widget* pChild : m_children)
		{
			if(pChild->IsEnabled())
				pChild->Draw(windowSize, scissor);
		}
	}

	void Widget::ReComputeSize(const DirectX::XMUINT2& parentSize)
	{
		if ((m_sizeStyle & HSIZE_STRETCH) != 0)
			m_size.x = parentSize.x - m_locPos.x;
		if ((m_sizeStyle & VSIZE_STRETCH) != 0)
			m_size.y = parentSize.y - m_locPos.y;

		//crop the size to the parent if the style is default
		if ((m_sizeStyle & HSIZE_DEFAULT) != 0 && m_defaultSize.x > parentSize.x - m_locPos.x)
			m_size.x = parentSize.x - m_locPos.x;
		if ((m_sizeStyle & VSIZE_STRETCH) != 0 && m_defaultSize.y > parentSize.y - m_locPos.y)
			m_size.y = parentSize.y - m_locPos.y;
	}

	void Widget::ReComputeSize_PostChildren()
	{
		if (m_sizeStyle & Widgets::Widget::HSIZE_FIT)
		{
			if (m_children.empty())
			{
				m_size.x = m_defaultSize.x;
			}
			else
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
			}
		}
		if (m_sizeStyle & Widgets::Widget::VSIZE_FIT)
		{
			if (m_children.empty())
			{
				m_size.y = m_defaultSize.y;
			}
			else
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
			}
		}
	}

	void Widget::ReComputePosition(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		switch (m_hPositionStyle)
		{
		case HPOSITION_STYLE::NONE:
			m_absPos.x = parentAbsPos.x + m_locPos.x;
			break;

		case HPOSITION_STYLE::LEFT:
			m_absPos.x = parentAbsPos.x;
			break;

		case HPOSITION_STYLE::CENTER:
			m_absPos.x = parentAbsPos.x + (parentSize.x / 2) - (m_size.x / 2);
			break;

		case HPOSITION_STYLE::RIGHT:
			m_absPos.x = parentAbsPos.x + parentSize.x - m_size.x;
			break;
		}

		switch (m_vPositionStyle)
		{
		case VPOSITION_STYLE::NONE:
			m_absPos.y = parentAbsPos.y + m_locPos.y;
			break;

		case VPOSITION_STYLE::TOP:
			m_absPos.y = parentAbsPos.y;
			break;

		case VPOSITION_STYLE::MIDDLE:
			m_absPos.y = parentAbsPos.y + (parentSize.y / 2) - (m_size.y / 2);
			break;

		case VPOSITION_STYLE::BOTTOM:
			m_absPos.y = parentAbsPos.y + parentSize.y - m_size.y;
			break;
		}

		m_absPos.z = parentAbsPos.z - 1;
	}

	void Widget::ResizeChildren()
	{
		for (Widget* pChild : m_children)
		{
			if (!pChild->IsEnabled())
				continue;
			pChild->Resize(m_absPos, m_size);
		}
	}

	void Widget::Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		ReComputeSize(parentSize);
		ReComputePosition(parentAbsPos, parentSize);

		ResizeChildren();

		ReComputeSize_PostChildren();
	}

	bool Widget::Handle(const BaseEvent& event)
	{
		bool handled = false;

		switch (event.m_id)
		{
		case EventType::kMouseEnter:
			m_hover = true;
			if (m_onMouseEnter)
			{
				const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(event);
				m_onMouseEnter(mouseEvent);
				return true;
			}
			break;

		case EventType::kMouseExit:
			m_hover = false;
			if (m_onMouseExit)
			{
				const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(event);
				m_onMouseExit(mouseEvent);
				return true;
			}
			break;

		case EventType::kMouseDown:
		{
			const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(event);
			if (m_onMouseDown)
			{
				m_onMouseDown(mouseEvent);
				handled = true;
			}
		}
		break;

		case EventType::kMouseUp:
		{
			const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(event);
			if (m_onMouseUp)
			{
				m_onMouseUp(mouseEvent);
				handled = true;
			}
			break;
		}

		case EventType::kMouseDoubleClick:
		{
			if (m_onMouseDoubleClick)
			{
				const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(event);
				m_onMouseDoubleClick(mouseEvent);
				handled = true;
			}
			break;
		}
		break;

		default:
			break;
		}

		return handled;
	}

	void Widget::AddWidget(Widget* pWidget)
	{
		m_children.push_back(pWidget);
		WidgetMgr::Get().RegisterWidget(pWidget);
		pWidget->m_pParent = this;
	}

	void Widget::InsertWidget(Widget* pWidget, int position)
	{
		std::vector<Widget*>::iterator it = m_children.begin();
		std::advance(it, position);

		m_children.insert(it, pWidget);
		WidgetMgr::Get().RegisterWidget(pWidget);
		pWidget->m_pParent = this;
	}

	void Widget::DeleteChild(Widget* pWidget)
	{
		WidgetMgr::Get().UnregisterWidget(pWidget);

		std::vector<Widget*>::const_iterator it = std::find(m_children.cbegin(), m_children.cend(), pWidget);
		if (it == m_children.cend())
			return;

		m_children.erase(it);
		pWidget->DeleteAllChildren();
		
		delete pWidget;
	}

	void Widget::DeleteAllChildren()
	{
		for (Widget* pChild : m_children)
		{
			WidgetMgr::Get().UnregisterWidget(pChild);
			pChild->DeleteAllChildren();
			delete pChild;
		}

		m_children.clear();
	}

	void Widget::Enable(bool recursive)
	{
		m_enabled = true;

		if (recursive)
		{
			for (Widget* pWidget : m_children)
				pWidget->Enable(recursive);
		}
	}

	void Widget::Disable(bool recursive)
	{
		m_enabled = false;

		if (recursive)
		{
			for (Widget* pWidget : m_children)
				pWidget->Disable(recursive);
		}
	}

	void Widget::SetX(int32_t x)
	{
		m_locPos.x = x;
	}

	void Widget::SetY(int32_t y)
	{
		m_locPos.y = y;
	}

	void Widget::SetSize(const DirectX::XMUINT2& size)
	{
		m_defaultSize = size;
		m_size = size;
	}

	void Widget::SetWidth(uint32_t width)
	{
		m_defaultSize.x = width;
		m_size.x = width;
	}

	void Widget::SetFocus()
	{
		Widgets::WidgetMgr& widgetMgr = Widgets::WidgetMgr::Get();
		widgetMgr.SetFocus(this);
	}

	void Widget::SetBackgroundColor(const DirectX::XMVECTOR& color)
	{
		m_backgroundColor = color;
	}

	void Widget::SetSizeStyle(int sizeStyle)
	{
		m_sizeStyle = sizeStyle;
	}

	void Widget::SetPositionStyle(Widget::HPOSITION_STYLE hStyle, Widget::VPOSITION_STYLE vStyle)
	{
		m_hPositionStyle = hStyle;
		m_vPositionStyle = vStyle;
	}

	void Widget::SetName(const std::string& name)
	{
		m_name = name;
	}

	void Widget::SetFocusPolicy(FOCUS_POLICY policy)
	{
		m_focusPolicy = policy;
	}

	int32_t Widget::GetX() const
	{
		return m_locPos.x;
	}

	int32_t Widget::GetY() const
	{
		return m_locPos.y;
	}

	DirectX::XMINT2 Widget::GetPosition() const
	{
		DirectX::XMINT2 pos;
		pos.x = m_locPos.x;
		pos.y = m_locPos.y;
		return pos;
	}

	DirectX::XMUINT2 Widget::GetSize() const
	{
		return m_size;
	}

	int32_t Widget::GetScreenX() const
	{
		return m_absPos.x;
	}

	int32_t Widget::GetScreenY() const
	{
		return m_absPos.y;
	}

	uint32_t Widget::GetWidth() const
	{
		return m_size.x;
	}

	uint32_t Widget::GetHeight() const
	{
		return m_size.y;
	}

	Widget::FOCUS_POLICY Widget::GetFocusPolicy() const
	{
		return m_focusPolicy;
	}

	bool Widget::HasFocus() const
	{
		return Widgets::WidgetMgr::Get().GetFocusedWidget() == this;
	}

	const std::vector<Widget*>& Widget::GetChildren() const
	{
		return m_children;
	}

	uint32_t Widget::GetChildrenCount() const
	{
		return static_cast<uint32_t>(m_children.size());
	}

	Widget* Widget::GetParent()
	{
		return m_pParent;
	}

	bool Widget::IsInside(uint32_t screenX, uint32_t screenY) const
	{
		if (screenX > (uint32_t)GetScreenX() && screenX < GetScreenX() + GetWidth() &&
			screenY >(uint32_t)GetScreenY() && screenY < GetScreenY() + GetHeight())
		{
			return true;
		}

		return false;
	}

	bool Widget::IsEnabled() const
	{
		return m_enabled;
	}

	void Widget::CaptureMouse()
	{
		WidgetMgr::Get().CaptureMouse(this);
	}

	void Widget::ReleaseMouse()
	{
		WidgetMgr::Get().CaptureMouse(nullptr);
	}

	void Widget::ComputeWVPMatrix(const DirectX::XMFLOAT2& windowSize, DirectX::XMMATRIX& wvp) const
	{
		float width = static_cast<float>(m_size.x);
		float height = static_cast<float>(m_size.y);
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(width, height, 0);

		//position : top left corner
		float windowWidth = windowSize.x;
		float windowHeight = windowSize.y;

		//compute the position on the screen.
		//this is in screen space coordinate but the unit is a pixel
		//so the origin is the center of the screen and it goes from [-window size / 2; window size / 2]
		float x = (float)m_absPos.x + (width * 0.5f) - (windowWidth * 0.5f);
		float y = -m_absPos.y - (height * 0.5f) + (windowHeight * 0.5f);
		float z = (float)m_absPos.z;
		DirectX::XMMATRIX position = DirectX::XMMatrixTranslation(x, y, z);

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMVectorSet(0, 0, 10, 1), DirectX::XMVectorSet(0, 1, 0, 1));

		float projWidth = static_cast<float>(windowWidth);
		float projHeight = static_cast<float>(windowHeight);
		DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicLH(projWidth, projHeight, NEAR_CAMERA_PLANE, FAR_CAMERA_PLANE);

		wvp = DirectX::XMMatrixMultiply(scale, position);
		wvp = DirectX::XMMatrixMultiply(wvp, view);
		wvp = DirectX::XMMatrixMultiply(wvp, projection);
	}

	void Widget::Internal_Render()
	{}
}
