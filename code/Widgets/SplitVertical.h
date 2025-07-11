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
	
	class SplitVertical : public Widget
	{
	public:
		enum ResizePolicy
		{
			KeepRatio,		//each panel keeps the same percentage of the total space
			KeepLeftSize,	//left panel keeps constant size
			KeepRightSize	//right panel keeps constant size
		};

		SplitVertical();
		~SplitVertical();

		void Update(uint64_t dt) override;
		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;
		void Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize) override;

		void AddWidget(Widget* pWidget) override;

		void AddLeftPanel(Widget* pWidget);
		void AddRightPanel(Widget* pWidget);

		void SetLeftPanelWidth(int width);
		void SetRightPanelWidth(int width);

		void SetResizePolicy(ResizePolicy policy);

	private:
		Layout* m_pLayout;
		Container* m_pLeftContainer;
		Split* m_pSplit;
		Container* m_pRightContainer;

		bool m_splitDragged;

		ResizePolicy m_resizePolicy;
	};
}
