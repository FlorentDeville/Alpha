/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Button.h"

#include "../Mesh.h"
#include "../Window.h"
#include "../RenderModule.h"

extern MeshId g_QuadMeshId;
extern Window* g_pWindow;
extern RenderModule* g_pRenderModule;

Button::Button(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: Widget(w, h, x, y)
{}

Button::~Button()
{}

void Button::Draw()
{
	//dimension : 100 * 20 pixels
	float width = static_cast<float>(m_width);
	float height = static_cast<float>(m_height);
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(width, height, 0);

	//position : top left corner
	float windowWidth = static_cast<float>(g_pWindow->GetWidth());
	float windowHeight = static_cast<float>(g_pWindow->GetHeight());

	float x = width * 0.5f - windowWidth * 0.5f + m_x;
	float y = windowHeight * 0.5f - height * 0.5f + m_y;

	DirectX::XMMATRIX position = DirectX::XMMatrixTranslation(x, y, 1);

	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMVectorSet(0, 0, 10, 1), DirectX::XMVectorSet(0, 1, 0, 1));

	float projWidth = static_cast<float>(windowWidth);
	float projHeight = static_cast<float>(windowHeight);
	DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicLH(projWidth, projHeight, 0.1f, 100.f);

	DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(scale, position);
	mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, view);
	mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, projection);

	g_pRenderModule->Render(g_QuadMeshId, mvpMatrix);
}
