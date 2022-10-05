/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Label.h"

#include "Rendering/RenderModule.h"

#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Label::Label(int32_t locX, int32_t locY, float scale, const std::string& text)
		: Widget(0, 0, locX, locY)
		, m_fontScale(scale)
		, m_text(text)
	{
		SetSizeStyle(Widget::HSIZE_STRETCH);
	}

	Label::~Label()
	{}

	void Label::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		DirectX::XMFLOAT3 uiPos((float)m_absPos.x, (float)m_absPos.y, (float)m_absPos.z);
		DirectX::XMUINT4 scissor(m_absPos.x, m_absPos.y, m_size.x, m_size.y);
		RenderModule::Get().PrepareRenderText(m_text, WidgetMgr::Get().GetUIFontId(), uiPos, DirectX::XMFLOAT2(m_fontScale, m_fontScale), scissor);
	}
}
