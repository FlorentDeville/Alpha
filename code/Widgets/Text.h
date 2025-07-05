/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include <string>

namespace Widgets
{
	class Text : public Widget
	{
	public:
		Text(float scale, const std::string& text);
		virtual ~Text();

		void Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor) override;

		void AppendText(const std::string& textToAppend);

	private:
		float m_fontScale;
		std::string m_text;
	};
}
