/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Container_V2 : public Widget
	{
	public:
		using Parent = Widget;

		Container_V2();
		~Container_V2();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;
		bool Handle(const BaseEvent& ev) override;
		void ResizeChildren() override;

	private:
		Core::Int2 m_absPosOffset;
		Core::UInt2 m_virtualSize;		//size propagated to the children
	};
}
