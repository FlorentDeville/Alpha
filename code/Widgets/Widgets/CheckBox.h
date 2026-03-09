/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Container.h"

#include "Core/Callbacks/CallbackMacro.h"

namespace Widgets
{
	class Label;

	class CheckBox : public Container
	{
		using Parent = Container;

	public:
		CheckBox(const std::string& text, bool value);
		~CheckBox();

		//void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;

		void SetValue(bool value);

		//bool Handle(const BaseEvent& ev) override;
		//void Enable(bool recursive = true) override;
		//void Disable(bool recursive = true) override;


		//void AddOption(const std::string& label, int64_t value);
		//void SetSelection(int64_t value);
		//void SetReadOnly(bool readOnly);

		EVENT_DECL(ValueChanged, void(bool newValue))

	private:
		bool m_value;

		Label* m_pLabel;
	};
}
