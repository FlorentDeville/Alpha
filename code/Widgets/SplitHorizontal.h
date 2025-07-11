/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Container;
	class Layout;
	class Split;

	class SplitHorizontal : public Widget
	{
	public:
		SplitHorizontal();
		~SplitHorizontal();

		void Update(uint64_t dt) override;
		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;
		void Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize) override;

		void AddWidget(Widget* pWidget) override;

		void AddTopPanel(Widget* pWidget);
		void AddBottomPanel(Widget* pWidget);

		void SetTopPanelHeight(int width);

	private:
		Layout* m_pLayout;
		Container* m_pTopContainer;
		Split* m_pSplit;
		Container* m_pBottomContainer;

		bool m_splitDragged;
	};
}
