/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

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

		void AddWidget(Widget* pWidget) override;
		void InsertWidget(Widget* pWidget, int position) override;
		void DeleteChild(Widget* pWidget) override;
		void DeleteAllChildren() override;

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;
		bool Handle(const GlobalEvent& ev) override;
		void Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize) override;
		void ResizeChildren() override;
		void ReComputePosition(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize) override;

		//Vertical scroll so the relative position inside the container is visible.
		void ScrollToHeight(uint32_t height);

	protected:
		Core::Int2 m_absPosOffset;
		Core::UInt2 m_virtualSize;		//size propagated to the children

		const int32_t SCROLL_CONTAINER_SIZE = 20;

		bool m_showVScrollBar;
		bool m_showHScrollBar;

	private:
		Core::Array<Widget*> m_externalWidgets;

		Core::UInt2 m_scrollingDistance;

		const int32_t SCROLLBAR_CONTAINER_SIZE = 10;

		//dragging
		bool m_isDraggingScrollBar;
		Core::Int2 m_draggingStartMousePos;
		Core::Int2 m_draggingStartPosOffset;

		//vertical scroll bar
		Container* m_pVScrollContainer;
		Container* m_pVScrollBar;
		int32_t m_vScrollContainerSize;			

		//horizontal scroll bar
		Container* m_pHScrollContainer;
		Container* m_pHScrollBar;
		int32_t m_hScrollContainerSize;

		void UpdateVScrollBarPositionFromOffset();

		void VScrollBar_OnMouseDown(const Widgets::MouseEvent& ev);
		void VScrollBar_OnMouseUp(const Widgets::MouseEvent& ev);
		void VScrollBar_OnMouseMove(const Widgets::MouseEvent& ev);

		void HScrollBar_OnMouseDown(const Widgets::MouseEvent& ev);
		void HScrollBar_OnMouseUp(const Widgets::MouseEvent& ev);
		void HScrollBar_OnMouseMove(const Widgets::MouseEvent& ev);
	};
}
