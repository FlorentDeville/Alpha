/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widget.h"

#include "../Window.h"

extern Window* g_pWindow;

Widget::Widget()
	: m_children()
	, m_width(0)
	, m_height(0)
	, m_x(0)
	, m_y(0)
{}

Widget::Widget(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: m_children()
	, m_width(w)
	, m_height(h)
	, m_x(x)
	, m_y(y)
{}

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

void Widget::AddWidget(Widget* pWidget)
{
	m_children.push_back(pWidget);
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

uint32_t Widget::GetWidth() const
{
	return m_width;
}

uint32_t Widget::GetHeight() const
{
	return m_height;
}

void Widget::ComputeWorldPosition(int32_t parentX, int32_t parentY, int32_t& x, int32_t& y) const
{
	//float width = static_cast<float>(m_width);
	//float height = static_cast<float>(m_height);

	//float windowWidth = static_cast<float>(g_pWindow->GetWidth());
	//float windowHeight = static_cast<float>(g_pWindow->GetHeight());

	//int32_t halfWidth = static_cast<int32_t>(width * 0.5f);
	//int32_t halfHeight = static_cast<int32_t>(height * 0.5f);

	x = m_x + parentX; //+ halfWidth;
	y = parentY - m_y; //- halfHeight;
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
