/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

#include "Widgets/Widget.h"

namespace Widgets
{
	class BaseEvent;

	class Split : public Widget
	{
	public:
		Split(bool isVerticalSplit);
		~Split();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;
		bool Handle(const BaseEvent& ev) override;

		bool IsDragged();
		Core::Int2 GetPreviousCursorPosition() const;

		void SetPreviousCursorPosition(const Core::Int2& pos);

		EVENT_DECL(Drag, void(const Core::Int2& offset))

	private:
		bool m_isDragged;
		Core::Int2 m_previousCursorPosition;

		bool m_isVerticalSplit;
	};
}
