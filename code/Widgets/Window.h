/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

#include <DirectXMath.h>

namespace Widgets
{
	class Container;

	class Window : public Widget
	{
	public:
		Window(DirectX::XMUINT2 size);
		~Window();

		void Update() override;
		void Draw(const DirectX::XMFLOAT2& windowSize) override;

		void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

		void AddWidget(Widget* pWidget) override;
		void RemoveWidget(const Widget* pWidget) override;

	private:
		DirectX::XMINT2 m_previousMousePosition;
		bool m_isMaximized;
		bool m_drag;

		Container* m_pContainer;
	};
}
