/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Label.h"

#include "Rendering/Font/FontMgr.h"
#include "Rendering/RenderModule.h"

#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Label::Label()
		: Widget(0, 0, 0, 0)
		, m_fontScale(1)
		, m_text()
	{
		SetSizeStyle(Widget::HSIZE_STRETCH);
	}

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
		RenderModule::Get().PrepareRenderText(m_text, WidgetMgr::Get().GetUIFontId(), uiPos, DirectX::XMFLOAT2(m_fontScale, m_fontScale), scissor, Widget::NEAR_CAMERA_PLANE, Widget::FAR_CAMERA_PLANE);
	}

	void Label::ReComputeSize(const DirectX::XMUINT2& parentSize)
	{
		if ((m_sizeStyle & Widget::HSIZE_FIT) || (m_sizeStyle & Widget::VSIZE_FIT))
		{
			Rendering::FontId fid = WidgetMgr::Get().GetUIFontId();
			const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(fid);

			DirectX::XMUINT2 textSize;
			pFont->ComputeRect(m_text, textSize);

			if (textSize.x > parentSize.x - m_locPos.x)
			{
				textSize.x = parentSize.x - m_locPos.x;
			}
			if (textSize.y > parentSize.y - m_locPos.y)
			{
				textSize.y = parentSize.y - m_locPos.y;
			}

			m_size = textSize;
		}
		else
		{
			Widgets::Widget::ReComputeSize(parentSize);
		}
	}

	void Label::ReComputeSize_PostChildren()
	{
		//don't do anything
	}

	void Label::SetFontScale(float scale)
	{
		m_fontScale = scale;
	}

	void Label::SetText(const std::string& text)
	{
		m_text = text;
	}
}
