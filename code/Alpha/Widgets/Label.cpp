/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Label.h"

#include "Rendering/RenderModule.h"

#include "Widgets/WidgetMgr.h"

extern WidgetMgr* g_pWidgetMgr;

Label::Label(int32_t locX, int32_t locY, float scale, const std::string& text)
	: Widget(0, 0, locX, locY)
	, m_fontScale(scale)
	, m_text(text)
{}

Label::~Label()
{}

void Label::Draw()
{
	DirectX::XMFLOAT3 uiPos((float)m_absPos.x, (float)m_absPos.y, (float)m_absPos.z);
	g_pRenderModule->PrepareRenderText(m_text, g_pWidgetMgr->m_segoeUIFontId, uiPos, DirectX::XMFLOAT2(m_fontScale, m_fontScale));
}
