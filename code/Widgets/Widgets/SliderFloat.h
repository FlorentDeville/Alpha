/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"
#include "Widgets/Style/SliderFloat/SliderFloatStyle.h"

namespace Widgets
{
	class Container;
	class TextBox;

	class SliderFloat : public Widget
	{
	public:
		SliderFloat(float minValue, float maxValue, float currentValue);
		~SliderFloat();

		//bool Handle(const BaseEvent& ev) override;

		void Update(uint64_t dt) override;
		void Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor) override;

		EVENT_DECL(Validate, void(const float value))

	private:

		enum InputMode
		{
			Slider,		//the user can drag the slider to change the value
			Text,		//the user can type value

			Count
		};

		float m_minValue;
		float m_maxValue;
		float m_currentValue;

		InputMode m_mode;

		Container* m_pSlider;
		TextBox* m_pTextbox;

		bool m_isSliderDragging;
		int m_mouseDragPreviousX;

		SliderFloatStyle m_defaultStyle;
		SliderFloatStyle m_hoverStyle;
		SliderFloatStyle m_hoverSliderStyle;

		int CalculateSliderLocalX();

		void DrawBackground(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& localScissor);
		void DrawSliderText(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& localScissor);

		void TransitionTextToSlider(bool valueChanged);
		void TransitionSliderToText();
	};
}
