/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include <string>

namespace Widgets
{
	class Label : public Widget
	{
	public:
		Label();
		Label(int32_t locX, int32_t locY, float scale, const std::string& text);
		virtual ~Label();

		void Draw(const DirectX::XMFLOAT2& windowSize) override;

		void SetFontScale(float scale);
		void SetText(const std::string& text);

	private:
		float m_fontScale;
		std::string m_text;
	};
}
