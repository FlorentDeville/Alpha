/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include "Widgets/Style/Container/ContainerStyle.h"

namespace Widgets
{

	class Container : public Widget
	{
	public:
		Container();
		Container(int width, int height);
		~Container();

		void Draw(const DirectX::XMFLOAT2& windowSize) override;

		ContainerStyle& GetDefaultStyle();

	private:
		ContainerStyle m_defaultStyle;
	};

}