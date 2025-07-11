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
		Label(const std::string& text);
		Label(int32_t locX, int32_t locY, float scale, const std::string& text);
		virtual ~Label();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;

		void ReComputeSize(const Core::UInt2& parentSize) override;
		void ReComputeSize_PostChildren() override;

		void SetFontScale(float scale);
		void SetText(const std::string& text);

	private:
		float m_fontScale;
		std::string m_text;
	};
}
