/********************************************************************/
/* � 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widgets/AbstractClickable.h"
#include "Widgets/Style/Button/ButtonStyle.h"

#include <functional>

namespace Widgets
{
	class Button : public AbstractClickable
	{
	public:
		using ParentClass = AbstractClickable;

		Button(uint32_t w, uint32_t h, int32_t x, int32_t y);
		Button(const std::string& textLabel);
		Button(const std::string& textLabel, uint32_t width, uint32_t height);

		virtual ~Button();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;

		bool Handle(const BaseEvent& event) override;

		void Select();
		void Unselect();

		ButtonStyle& GetHoverStyle();

		bool IsSelected() const;

	private:
		//if the button is selected, it keeps the hover color until Unselect is called.
		bool m_isSelected;

		//one style per state
		ButtonStyle m_defaultStyle;
		ButtonStyle m_hoverStyle;
		ButtonStyle m_selectedStyle;
	};
}
