/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Container.h"

namespace Widgets
{
	class Layout;

	class Menu : public Container
	{
	public:
		Menu();
		~Menu();

		bool Handle(const Message& msg) override;
		void ReComputePosition(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

		void AddMenuItem(const std::string& itemName);

	private:
		Layout* m_pLayout;
	};
}
