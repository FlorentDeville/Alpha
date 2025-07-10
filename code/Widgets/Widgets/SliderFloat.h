/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"
#include "Widgets/Style/SliderFloat/SliderFloatStyle.h"

namespace Widgets
{
	class Container;

	class SliderFloat : public Widget
	{
	public:
		SliderFloat(float minValue, float maxValue, float currentValue);
		~SliderFloat();

		//bool Handle(const BaseEvent& ev) override;

		void Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor) override;

	private:

		float m_minValue;
		float m_maxValue;
		float m_currentValue;

		Container* m_pSlider;

		SliderFloatStyle m_defaultStyle;
		SliderFloatStyle m_hoverStyle;
		SliderFloatStyle m_hoverSliderStyle;
	};
}
