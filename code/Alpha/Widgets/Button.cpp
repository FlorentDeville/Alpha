/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Button.h"

#include "Rendering/Mesh.h"

#include "Rendering/RenderModule.h"
#include "Rendering/Renderable/RenderableMgr.h"

extern MeshId g_QuadMeshId;
//extern Window* g_pWindow;
extern RenderModule* g_pRenderModule;

Button::Button(uint32_t w, uint32_t h, int32_t x, int32_t y)
	: Widget(w, h, x, y)
{}

Button::~Button()
{}

void Button::Draw(int32_t parentX, int32_t parentY, float parentZ)
{
	DirectX::XMMATRIX mvpMatrix;
	ComputeWVPMatrix(mvpMatrix, parentX, parentY, parentZ);
	g_pRenderModule->Render(*g_pRenderableMgr->GetQuad(), mvpMatrix);
}
