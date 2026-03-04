/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Container.h"

#include "Core/Collections/Array.h"

namespace Widgets
{
	class Label;

	class ComboBox : public Container
	{
	public:
		ComboBox();
		~ComboBox();

		//void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;
		//void Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize) override;

		bool Handle(const BaseEvent& ev) override;

		void AddOption(const std::string& label, int64_t value);

		void SetSelection(int64_t value);

	private:
		struct Option
		{
			std::string m_label;
			int64_t m_value;
		};

		Core::Array<Option> m_options;
		uint32_t m_selectedOptionIndex;

		Label* m_pLabel;
	};
}
