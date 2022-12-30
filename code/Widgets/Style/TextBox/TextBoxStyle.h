/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Style/BaseStyle.h"

#include <DirectXMath.h>

namespace Widgets
{
	class TextBoxStyle : public BaseStyle
	{
		friend class TextBox;

	public:
		TextBoxStyle();
		~TextBoxStyle();

		void SetShowBorder(bool show);
		void SetBackgroundColor(const DirectX::XMVECTOR& color);
		void SetBorderColor(const DirectX::XMVECTOR& color);
		void SetTextColor(const DirectX::XMVECTOR& color);
		void SetBorderSize(int size);

	private:
		int m_borderSize;
		char m_padding[4];
		DirectX::XMVECTOR m_backgroundColor;
		DirectX::XMVECTOR m_borderColor;
		DirectX::XMVECTOR m_textColor;
		bool m_showBorder;
	};
}
