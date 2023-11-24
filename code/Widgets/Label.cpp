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
		m_focusPolicy = Widget::FOCUS_POLICY::NO_FOCUS;
	}

	Label::Label(const std::string& text)
		: Label(0, 0, 1, text)
	{
		Rendering::FontId fontId = WidgetMgr::Get().GetUIFontId();
		const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(fontId);
		DirectX::XMUINT2 size;
		pFont->ComputeRect(text, size);
		SetSize(size);
		SetSizeStyle(Widget::HSIZE_STRETCH);
	}

	Label::Label(int32_t locX, int32_t locY, float scale, const std::string& text)
		: Widget(0, 0, locX, locY)
		, m_fontScale(scale)
		, m_text(text)
	{
		SetSizeStyle(Widget::HSIZE_STRETCH);
		m_focusPolicy = Widget::FOCUS_POLICY::NO_FOCUS;
	}

	Label::~Label()
	{}

	void Label::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		DirectX::XMFLOAT3 uiPos((float)m_absPos.x, (float)m_absPos.y, (float)m_absPos.z);
		DirectX::XMUINT4 scissor(m_absPos.x, m_absPos.y, m_size.x, m_size.y);
		Rendering::RenderModule::Get().PrepareRenderText(m_text, WidgetMgr::Get().GetUIFontId(), uiPos, DirectX::XMFLOAT2(m_fontScale, m_fontScale), scissor, Widget::NEAR_CAMERA_PLANE, Widget::FAR_CAMERA_PLANE);
	}

	void Label::ReComputeSize(const DirectX::XMUINT2& parentSize)
	{
		bool anyNotFit = ((m_sizeStyle & Widget::HSIZE_FIT) == 0) && ((m_sizeStyle & Widget::VSIZE_FIT) == 0);
		bool anyFit = (m_sizeStyle & Widget::HSIZE_FIT) || (m_sizeStyle & Widget::VSIZE_FIT);

		if (anyNotFit)
		{
			Widget::ReComputeSize(parentSize);
		}

		if (anyFit)
		{
			Rendering::FontId fid = WidgetMgr::Get().GetUIFontId();
			const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(fid);

			DirectX::XMUINT2 textSize;
			pFont->ComputeRect(m_text, textSize);

			if ((m_sizeStyle & Widget::HSIZE_FIT) != 0)
			{
				m_size.x = textSize.x;
			}

			if ((m_sizeStyle & Widget::VSIZE_FIT) != 0)
			{
				m_size.y = textSize.y;
			}
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
