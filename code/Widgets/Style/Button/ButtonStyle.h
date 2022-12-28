/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Style/BaseStyle.h"

#include <DirectXMath.h>

namespace Widgets
{
	class ButtonStyle : public BaseStyle
	{
		friend class Button;

	public:
		ButtonStyle();
		~ButtonStyle();

		void SetBorderSize(int size);
		void SetBackgroundColor(const DirectX::XMVECTOR& backgroundColor);
		void SetBorderColor(const DirectX::XMVECTOR& borderColor);
		void ShowBorder(bool showBorder);

	private:
		int m_borderSize;
		char m_padding[4];
		DirectX::XMVECTOR m_backgroundColor;
		DirectX::XMVECTOR m_borderColor;
		bool m_showBorder;
	};
}
