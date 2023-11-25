/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Container.h"

namespace Widgets
{
	class BaseEvent;
	class Layout;
	class MenuItem;

	//Menu display a list of MenuItems. The menu stays visible as long as it has the focus so everything inside of it must be
	// in focus policy NO_FOCUS, including the MenuItems.
	class Menu : public Container
	{
	public:
		Menu();
		~Menu();

		bool Handle(const BaseEvent& ev) override;
		void ReComputePosition(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

		MenuItem* AddMenuItem(const std::string& itemName);

	private:
		Layout* m_pLayout;
	};
}
