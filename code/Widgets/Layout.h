/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"
#include "Widgets/Style/Layout/LayoutStyle.h"

namespace Widgets
{
	class Layout : public Widget
	{
	public:
		enum Direction
		{
			Horizontal,				//left to right
			Horizontal_Reverse,		//right to left
			Vertical,				//top to bottom
			Vertical_Reverse		//bottom to top
		};

		Layout();
		Layout(uint32_t w, uint32_t h, int32_t x, int32_t y);
		Layout(Direction dir, Widget::SIZE_STYLE sizeStyle);

		virtual ~Layout();

		void Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize) override;

		void ReComputeSize_PostChildren() override;

		void Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor) override;

		bool Handle(const BaseEvent& ev) override;

		void SetDirection(Direction dir);
		void SetSpace(const DirectX::XMINT2& space);
		void SetTransparent(bool transparent); //if true, the layout doesn't display any background nor border

		LayoutStyle& GetDefaultStyle();
		LayoutStyle& GetHoverStyle();

	private:
		Direction m_dir;

		DirectX::XMUINT2 m_previousMousePosition;

		DirectX::XMINT2 m_space; //space to add between widgets

		LayoutStyle m_defaultStyle;
		LayoutStyle m_hoverStyle;

		bool m_transparent; //if true, the layout doesn't display any background nor border
	};
}
