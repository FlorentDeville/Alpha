/********************************************************************/
/* � 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class BaseEvent;

	class Split : public Widget
	{
	public:
		Split(bool isVerticalSplit);
		~Split();

		void Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor) override;
		bool Handle(const BaseEvent& ev) override;

		bool IsDragged();
		DirectX::XMINT2 GetPreviousCursorPosition() const;

		void SetPreviousCursorPosition(const DirectX::XMINT2& pos);

	public:
		bool m_isDragged;
		DirectX::XMINT2 m_previousCursorPosition;

		bool m_isVerticalSplit;
	};
}
