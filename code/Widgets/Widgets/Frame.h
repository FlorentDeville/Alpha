/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Container.h"

#include "Core/Callbacks/CallbackList.h"

#include <string>

namespace Widgets
{
	class Icon;
	class Label;
	class Layout;
	class Widget;

	class Frame : public Container
	{
	public:
		Frame(const std::string& title);
		~Frame();

		void AddWidget(Widget* pWidget) override;

		void Close();

		using OnCloseEvent = Core::CallbackList<void()>;

		Core::CallbackId OnClose(const OnCloseEvent::Callback& callback);

	private:		
		Layout* m_pInternalLayout;
		Label* m_pTitleLabel;
		Layout* m_pTitleLayout;
		Container* m_pInternalContainer;
		Icon* m_pCloseIcon;
		Icon* m_pCloseHoverIcon;

		OnCloseEvent m_onClose;

		void OnClick_CloseButton();
		bool OnMouseEnter_CloseButton();
		bool OnMouseExit_CloseButton();
	};
}
