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

		void Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor) override;

		void Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize) override;
		void ResizeChildren() override;
		void ReComputeSize_PostChildren() override;

		void Enable(bool recursive = true) override;
		void Disable(bool recursive = true) override;

	private:
		Layout* m_pLayout;
		std::vector<Button*> m_buttonsArray;
		std::vector<Menu*> m_menusArray;

		bool OnClick_MenuButton(Button* pButton, Menu* pMenu);
		void OnLoseFocus_Menu(const FocusEvent& ev, Button* pButton, Menu* pMenu);
		void OnOpen_Menu(Button* pButton);
		void OnClose_Menu(Button* pButton);
	};
}
