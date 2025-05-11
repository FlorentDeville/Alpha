/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Container.h"

namespace Widgets
{
	class BaseEvent;
	class Label;
	class Layout;

	class ModalWindow : public Container
	{
	public:
		ModalWindow(const std::string& title);
		~ModalWindow();

		void ReComputePosition(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

		bool Handle(const BaseEvent& ev) override;

		void AddWidget(Widget* pWidget) override;
		void DeleteChild(Widget* pWidget) override;

		virtual void Open();
		virtual void Close();

		// Height of the title bar of the modal window
		const int TITLE_HEIGHT = 20;

	private:
		Widgets::Layout* m_pInnerLayout;
		Widgets::Label* m_pTitleLabel;
		Widgets::Container* m_pContent;
	};
}
