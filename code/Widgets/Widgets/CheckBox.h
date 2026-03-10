/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Container.h"

#include "Core/Callbacks/CallbackMacro.h"

namespace Widgets
{
	class Container;
	class Icon;
	class Label;

	class CheckBox : public Container
	{
		using Parent = Container;

	public:
		CheckBox(const std::string& text, bool value);
		~CheckBox();

		void ReComputeSize(const Core::UInt2& parentSize) override;
		void Enable(bool recursive = true) override;

		void SetValue(bool value);

		void ShowLabel(bool show);
		


		//void AddOption(const std::string& label, int64_t value);
		//void SetSelection(int64_t value);
		//void SetReadOnly(bool readOnly);

		EVENT_DECL(ValueChanged, void(bool newValue))

	private:
		bool m_value;
		bool m_showLabel;

		Label* m_pLabel;
		Container* m_pContainer;
		Icon* pCheckedIcon;

		void Internal_OnMouseDown(const MouseEvent& ev);
	};
}
