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
		: Widget(0, 20, 0, 0)
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
		Rendering::FontId fontId = WidgetMgr::Get().GetUIFontId();
		const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(fontId);
		DirectX::XMUINT2 size;
		pFont->ComputeRect(text, size);
		SetSize(size);

		SetSizeStyle(Widget::HSIZE_STRETCH);
		m_focusPolicy = Widget::FOCUS_POLICY::NO_FOCUS;
	}

	Label::~Label()
	{}

	void Label::Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor)
	{
		DirectX::XMFLOAT3 uiPos((float)m_absPos.x, (float)m_absPos.y, (float)m_absPos.z);

		D3D12_RECT localScissorRect;
		localScissorRect.left = m_absPos.x;
		localScissorRect.right = m_absPos.x + m_size.x;
		localScissorRect.top = m_absPos.y;
		localScissorRect.bottom = m_absPos.y + m_size.y;

		//Discard the text if it goes out of the scissor vertically. I still send the
		//render request if it goes out horizontally because the rendering will only discard the
		//characters outside.
		if (localScissorRect.bottom > scissor.bottom)
			return;

		//intersect the scissor with the box of the widget
		if (localScissorRect.left < scissor.left) localScissorRect.left = scissor.left;
		if (localScissorRect.top < scissor.top) localScissorRect.top = scissor.top;
		if (localScissorRect.right > scissor.right) localScissorRect.right = scissor.right;

		if (localScissorRect.left >= localScissorRect.right || localScissorRect.top >= localScissorRect.bottom)
			return;

		DirectX::XMUINT4 localScissor(localScissorRect.left, localScissorRect.top, localScissorRect.right - localScissorRect.left, localScissorRect.bottom - localScissorRect.top);
		Rendering::RenderModule::Get().PrepareRenderText(m_text, WidgetMgr::Get().GetUIFontId(), uiPos, DirectX::XMFLOAT2(m_fontScale, m_fontScale), localScissor, Widget::NEAR_CAMERA_PLANE, Widget::FAR_CAMERA_PLANE);
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
