/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"
#include "Widgets/Split.h"
#include "Widgets/Layout.h"
#include "Widgets/Container.h"

namespace Widgets
{
	class Split3Way : public Widget
	{
	public:
		Split3Way();
		~Split3Way();

		void Update() override;
		void Draw() override;

	private:
		Layout* m_pLayout;
		Container* m_pLeftContainer;
		Split* m_pLeftSplit;
		Container* m_pMiddleContainer;
		Split* m_pRightSplit;
		Container* m_pRightContainer;

		bool m_leftDragged;
		bool m_rightDragged;
	};
}
