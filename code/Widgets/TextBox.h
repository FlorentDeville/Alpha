/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include "Widgets/Style/TextBox/TextBoxStyle.h"

namespace Widgets
{
	class Icon;

	class TextBox : public Widget
	{
	public:
		enum TextAlignment
		{
			Left,
			Center,
			Right
		};

		TextBox();
		~TextBox();

		void Update(uint64_t dt) override;
		void Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor) override;
		void ResizeChildren() override;

		bool Handle(const BaseEvent& ev) override;

		void SetText(const std::string& text);
		void SetTextAlignment(TextAlignment alignment);

		void SetReadOnly(bool readOnly);

		void Enable(bool recursive = true) override;

		const std::string& GetText() const;

		EVENT_DECL(Validate, void(const std::string& value))
		EVENT_DECL(Escape, void())

	private:
		enum State
		{
			DEFAULT,
			HOVER,
			EDIT,
			READONLY
		};

		TextBoxStyle m_defaultStyle;
		TextBoxStyle m_editStyle;
		TextBoxStyle m_hoverStyle;
		TextBoxStyle m_readOnlyStyle;

		Icon* m_pCursorIcon;

		std::string m_text;

		State m_currentState;

		int m_cursorPosition;
		
		uint64_t m_cursorLastBlinkChange;
		
		TextAlignment m_textAlignment;

		DirectX::XMUINT2 m_textLocalPos;

		bool OnGetFocusCallback();
		bool OnLoseFocusCallback();

		void ComputeCursorPosition();
		void ComputeTextLocalPosition();

		void DrawText(const D3D12_RECT& scissor);
	};
}
