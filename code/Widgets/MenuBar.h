/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Button;
	class Layout;
	class Menu;

	class MenuBar : public Widget
	{
	public:
		MenuBar();
		~MenuBar();

		Menu* AddMenu(const std::string& name);

		void Draw(const DirectX::XMFLOAT2& windowSize) override;
	
		void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;
		void ResizeChildren() override;
		void ReComputeSize_PostChildren() override;

		void Enable(bool recursive = true) override;
		void Disable(bool recursive = true) override;

	private:
		Layout* m_pLayout;
		std::vector<Button*> m_buttonsArray;
		std::vector<Menu*> m_menusArray;

		bool OnClick_MenuButton(Button* pButton, Menu* pMenu);
		bool OnLoseFocus_Menu(Button* pButton, Menu* pMenu);
	};
}
