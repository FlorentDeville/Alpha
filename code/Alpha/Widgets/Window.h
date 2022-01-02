/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include <DirectXMath.h>

namespace Widgets
{
	class Window : public Widget
	{
	public:
		Window(DirectX::XMUINT2 size);
		~Window();

		void Draw();

		void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize);

	private:
		DirectX::XMINT2 m_previousMousePosition;
		bool m_isMaximized;
	};
}
