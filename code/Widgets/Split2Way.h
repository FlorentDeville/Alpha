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
	
	class Split2Way : public Widget
	{
	public:
		Split2Way();
		~Split2Way();

		void Update() override;
		void Draw(const DirectX::XMFLOAT2& windowSize) override;
		void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

		void AddWidget(Widget* pWidget) override;

		void AddLeftPanel(Widget* pWidget);
		void AddRightPanel(Widget* pWidget);

		void SetLeftPanelWidth(int width);

	private:
		Layout* m_pLayout;
		Container* m_pLeftContainer;
		Split* m_pSplit;
		Container* m_pRightContainer;

		bool m_splitDragged;
	};
}
