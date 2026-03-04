/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Container.h"

#include "Core/Callbacks/CallbackMacro.h"
#include "Core/Collections/Array.h"

namespace Widgets
{
	class Label;
	class Layout;

	class ComboBox : public Container
	{
		using Parent = Container;

	public:
		ComboBox();
		~ComboBox();

		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;

		bool Handle(const BaseEvent& ev) override;
		void Enable(bool recursive = true) override;
		void Disable(bool recursive = true) override;


		void AddOption(const std::string& label, int64_t value);
		void SetSelection(int64_t value);

		EVENT_DECL(SelectionChanged, void(int64_t oldValue, int64_t newValue))

	private:
		struct Option
		{
			std::string m_label;
			int64_t m_value;
		};

		Core::Array<Option> m_options;
		uint32_t m_selectedOptionIndex;

		Label* m_pLabel;

		Container* m_pOptionsContainer;
		Layout* m_pOptionsLayout;

		bool m_displayOptions;

		void InternalSetSelection(uint32_t optionIndex);
	};
}
