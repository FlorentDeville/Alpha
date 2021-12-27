/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widget.h"

#include "../Window.h"

#include "Widgets/WidgetMgr.h"
struct Message;

extern Window* g_pWindow;

Widget::Widget()
	: m_children()
	, m_width(0)
	, m_height(0)
	, m_x(0)
	, m_y(0)
	, m_screenX(0)
	, m_screenY(0)
	, m_hover(false)
	, m_backgroundColor(DirectX::XMVectorSet(0.12f, 0.12f, 0.12f, 1.f))
	, m_borderColor(DirectX::XMVectorSet(1.f, 1.f, 1.f, 1.f))
	, m_showBorder(false)
	, m_borderWidth(1)
{}

Widget::Widget(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: Widget()
{
	m_width = w;
	m_height = h;
	m_x = x;
	m_y = y;
}

Widget::~Widget()
{
	for (const Widget* pWidget : m_children)
		delete pWidget;
}

void Widget::Update()
{}

void Widget::Draw(int32_t /*x*/, int32_t /*y*/, float /*parentZ*/)
{}

void Widget::Resize()
{}

bool Widget::Handle(const Message& /*msg*/)
{
	return false;
}

void Widget::AddWidget(Widget* pWidget)
{
	m_children.push_back(pWidget);
	g_pWidgetMgr->RegisterWidget(pWidget);
}

void Widget::SetX(int32_t x)
{
	m_x = x;
}

void Widget::SetY(int32_t y)
{
	m_y = y;
}

int32_t Widget::GetX() const
{
	return m_x;
}

int32_t Widget::GetY() const
{
	return m_y;
}

int32_t Widget::GetScreenX() const
{
	return m_screenX;
}

int32_t Widget::GetScreenY() const
{
	return m_screenY;
}

uint32_t Widget::GetWidth() const
{
	return m_width;
}

uint32_t Widget::GetHeight() const
{
	return m_height;
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

void Widget::ComputeWorldPosition(int32_t parentX, int32_t parentY, int32_t& x, int32_t& y) const
{
	x = m_x + parentX;
	y = parentY - m_y;
}

void Widget::ComputeWVPMatrix(DirectX::XMMATRIX& wvp, int32_t parentX, int32_t parentY, float parentZ) const
{
	float width = static_cast<float>(m_width);
	float height = static_cast<float>(m_height);
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(width, height, 0);

	//position : top left corner
	float windowWidth = static_cast<float>(g_pWindow->GetWidth());
	float windowHeight = static_cast<float>(g_pWindow->GetHeight());

	float x = (float)m_x + parentX + width * 0.5f;
	float y = (float)parentY - m_y - height * 0.5f;
	float z = parentZ - 0.1f;
	DirectX::XMMATRIX position = DirectX::XMMatrixTranslation(x, y, z);

	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMVectorSet(0, 0, 10, 1), DirectX::XMVectorSet(0, 1, 0, 1));

	float projWidth = static_cast<float>(windowWidth);
	float projHeight = static_cast<float>(windowHeight);
	DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicLH(projWidth, projHeight, 0.1f, 100.f);

	wvp = DirectX::XMMatrixMultiply(scale, position);
	wvp = DirectX::XMMatrixMultiply(wvp, view);
	wvp = DirectX::XMMatrixMultiply(wvp, projection);
}

void Widget::ComputeScreenPosition(int32_t wx, int32_t wy, int32_t& screenX, int32_t& screenY)
{
	int32_t halfWindowWidth = static_cast<int32_t>(g_pWindow->GetWidth() * 0.5f);
	int32_t halfWindowHeight = static_cast<int32_t>(g_pWindow->GetHeight() * 0.5f);

	screenX = wx + halfWindowWidth;
	screenY = wy - halfWindowHeight;
}
