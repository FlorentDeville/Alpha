/********************************************************************/
/* � 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Text.h"

#include "Rendering/Font/FontMgr.h"
#include "Rendering/RenderModule.h"

#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Text::Text(float scale, const std::string& text)
		: Widget(0, 0, 0, 0)
		, m_fontScale(scale)
		, m_text(text)
	{}

	Text::~Text()
	{}

	void Text::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		Rendering::RenderModule& renderer = Rendering::RenderModule::Get();

		DirectX::XMFLOAT3 uiPos((float)m_absPos.x, (float)m_absPos.y, (float)m_absPos.z);
		DirectX::XMUINT4 localScissor(m_absPos.x, m_absPos.y, m_size.x, m_size.y);

		Rendering::FontId fontId = WidgetMgr::Get().GetUIFontId();
		const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(fontId);

		size_t startOfLine = 0;
		size_t endOfLine = 0;
		do
		{
			//first split by line
			endOfLine = m_text.find_first_of('\n', startOfLine);
			std::string line = m_text.substr(startOfLine, endOfLine - startOfLine);
			
			std::string subLine = line;

			//then split the line by the number of characters I can rander.
			int n = Rendering::RenderModule::Get().PrepareRenderText(subLine, WidgetMgr::Get().GetUIFontId(), uiPos, DirectX::XMFLOAT2(m_fontScale, m_fontScale), localScissor, Widget::NEAR_CAMERA_PLANE, Widget::FAR_CAMERA_PLANE);
			uiPos.y += pFont->m_lineHeight;
			while(n < subLine.size())
			{	
				subLine = subLine.substr(n + 1);
				n = Rendering::RenderModule::Get().PrepareRenderText(subLine, WidgetMgr::Get().GetUIFontId(), uiPos, DirectX::XMFLOAT2(m_fontScale, m_fontScale), localScissor, Widget::NEAR_CAMERA_PLANE, Widget::FAR_CAMERA_PLANE);
				uiPos.y += pFont->m_lineHeight;
			} 

			startOfLine = endOfLine + 1;
		} while (endOfLine != std::string::npos);
	}

	void Text::AppendText(const std::string& textToAppend)
	{
		m_text += textToAppend;
	}
}
