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
		using OnValidateCallback = std::function<bool(const std::string& value)>;

		TextBox();
		~TextBox();

		void Update(uint64_t dt) override;
		void Draw(const DirectX::XMFLOAT2& windowSize) override;
		void ResizeChildren() override;

		void Enable(bool recursive = true) override;

		bool Handle(const Message& msg) override;

		void SetText(const std::string& text);

		void OnValidate(const OnValidateCallback& callback);

	private:
		enum State
		{
			DEFAULT,
			HOVER,
			EDIT
		};

		TextBoxStyle m_defaultStyle;
		TextBoxStyle m_editStyle;
		TextBoxStyle m_hoverStyle;

		Icon* m_pCursorIcon;

		std::string m_text;

		State m_currentState;

		int m_cursorPosition;

		OnValidateCallback m_onValidateCallback;
		
		uint64_t m_cursorLastBlinkChange;

		bool OnGetFocusCallback();
		bool OnLoseFocusCallback();

		void ComputeCursorPosition();
	};
}
