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

		void Update() override;
		void Draw(const DirectX::XMFLOAT2& windowSize) override;
		void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

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
