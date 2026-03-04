/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include "Widgets/Style/Container/ContainerStyle.h"

namespace Widgets
{

	class Container : public Widget
	{
	public:
		Container();
		Container(int width, int height);
		~Container();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;

		bool Handle(const BaseEvent& ev) override;

		void ResizeChildren() override;

		ContainerStyle& GetDefaultStyle();
		ContainerStyle& GetHoverStyle();

	private:
		enum State
		{
			Default,
			Hover
		};

		State m_currentState;
		ContainerStyle m_defaultStyle;
		ContainerStyle m_hoverStyle;
	};

}