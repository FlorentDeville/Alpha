/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widget.h"

#include "SysWindow.h"

#include "Widgets/WidgetMgr.h"
#include "Widgets/Message.h"

extern SysWindow* g_pWindow;

Widget::Widget()
	: m_children()
	, m_size(0, 0)
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
{}

Widget::Widget(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: Widget()
{
	m_size.x = w;
	m_size.y = h;
	m_locPos = DirectX::XMINT3(x, y, 0);
}

Widget::~Widget()
{
	for (const Widget* pWidget : m_children)
		delete pWidget;
}

void Widget::Update()
{}

void Widget::Draw()
{
	for (Widget* pChild : m_children)
		pChild->Draw();
}

void Widget::ReComputeSize(const DirectX::XMUINT2& parentSize)
{
	if ((m_sizeStyle & HSIZE_STRETCH) != 0)
		m_size.x = parentSize.x - m_locPos.x;
	if ((m_sizeStyle & VSIZE_STRETCH) != 0)
		m_size.y = parentSize.y - m_locPos.y;
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

void Widget::Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
{
	ReComputeSize(parentSize);
	ReComputePosition(parentAbsPos, parentSize);

	for (Widget* pChild : m_children)
		pChild->Resize(m_absPos, m_size);
}

bool Widget::Handle(const Message& msg)
{
	switch(msg.m_id)
	{
	case M_MouseLDown:
		if (m_onLeftMouseDown)
			return m_onLeftMouseDown(msg.m_low.m_pos[0], msg.m_low.m_pos[1]);
		break;

	case M_MouseLUp:
		if (m_onLeftMouseUp)
			return m_onLeftMouseUp(msg.m_low.m_pos[0], msg.m_low.m_pos[1]);
		break;

	case M_MouseEnter:
		if (m_onMouseEnter)
			return m_onMouseEnter();
		break;

	case M_MouseExit:
		if (m_onMouseExit)
			return m_onMouseExit();
		break;

	default:
		break;
	}
	return false;
}

void Widget::AddWidget(Widget* pWidget)
{
	m_children.push_back(pWidget);
	g_pWidgetMgr->RegisterWidget(pWidget);
}

void Widget::RemoveWidget(const Widget* pWidget)
{
	std::vector<Widget*>::const_iterator it = std::find(m_children.cbegin(), m_children.cend(), pWidget);
	if (it == m_children.cend())
		return;

	m_children.erase(it);
}

void Widget::RemoveAllWidgets()
{
	m_children.clear();
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
	m_size = size;
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

bool Widget::IsInside(uint32_t screenX, uint32_t screenY) const
{
	if (screenX > (uint32_t)GetScreenX() && screenX < GetScreenX() + GetWidth() &&
		screenY > (uint32_t)GetScreenY() && screenY < GetScreenY() + GetHeight())
	{
		return true;
	}

	return false;
}

void Widget::CaptureMouse()
{
	g_pWidgetMgr->CaptureMouse(this);
}

void Widget::ReleaseMouse()
{
	g_pWidgetMgr->CaptureMouse(nullptr);
}

void Widget::OnMouseMove(const std::function<bool(int, int, MouseKey)>& callback)
{
	m_onMouseMove = callback;
}

void Widget::OnLeftMouseDown(const std::function<bool(int, int)>& callback)
{
	m_onLeftMouseDown = callback;
}

void Widget::OnLeftMouseUp(const std::function<bool(int, int)>& callback)
{
	m_onLeftMouseUp = callback;
}

void Widget::OnMouseEnter(const std::function<bool()>& callback)
{
	m_onMouseEnter = callback;
}

void Widget::OnMouseExit(const std::function<bool()>& callback)
{
	m_onMouseExit = callback;
}

void Widget::OnClick(const std::function<bool()>& callback)
{
	m_onClick = callback;
}

void Widget::ComputeWVPMatrix(DirectX::XMMATRIX& wvp) const
{
	float width = static_cast<float>(m_size.x);
	float height = static_cast<float>(m_size.y);
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(width, height, 0);

	//position : top left corner
	float windowWidth = static_cast<float>(g_pWindow->GetWidth());
	float windowHeight = static_cast<float>(g_pWindow->GetHeight());

	//compute the position on the screen.
	//this is in screen space coordinate but the unit is a pixel
	//so the origin is the center of the screen and it goes from [-window size / 2; window size / 2]
	float x = (float)m_absPos.x + (width * 0.5f) - (windowWidth * 0.5f);
	float y = - m_absPos.y - (height * 0.5f) + (windowHeight * 0.5f);
	float z = (float)m_absPos.z;
	DirectX::XMMATRIX position = DirectX::XMMatrixTranslation(x, y, z);

	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMVectorSet(0, 0, 10, 1), DirectX::XMVectorSet(0, 1, 0, 1));

	float projWidth = static_cast<float>(windowWidth);
	float projHeight = static_cast<float>(windowHeight);
	DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicLH(projWidth, projHeight, 0.1f, 100.f);

	wvp = DirectX::XMMatrixMultiply(scale, position);
	wvp = DirectX::XMMatrixMultiply(wvp, view);
	wvp = DirectX::XMMatrixMultiply(wvp, projection);
}
