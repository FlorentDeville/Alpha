/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Container;

	class Container_V2 : public Widget
	{
	public:
		using Parent = Widget;

		Container_V2();
		~Container_V2();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;
		bool Handle(const BaseEvent& ev) override;
		void Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize) override;
		void ResizeChildren() override;

	private:
		Core::Int2 m_absPosOffset;
		Core::UInt2 m_virtualSize;		//size propagated to the children
		Core::UInt2 m_scrollingDistance;

		const int SCROLL_CONTAINER_SIZE = 20;
		const int SCROLLBAR_CONTAINER_SIZE = 10;
		Container* m_pVerticalScrollContainer;
		Container* m_pVerticalScrollBar;

		bool m_showVerticalScrollBar;
		bool m_isDraggingVerticalScrollBar;
		Core::Int2 m_dragVScrollBarStartPos;

		void UpdateVScrollBarPositionFromOffset();

		void VScrollBar_OnMouseDown(const Widgets::MouseEvent& ev);
		void VScrollBar_OnMouseUp(const Widgets::MouseEvent& ev);
		void VScrollBar_OnMouseMove(const Widgets::MouseEvent& ev);
	};
}
