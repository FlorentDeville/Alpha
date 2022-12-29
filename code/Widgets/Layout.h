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
			//Vertical_Reverse		//bottom to top
		};

		Layout();
		Layout(uint32_t w, uint32_t h, int32_t x, int32_t y);
		virtual ~Layout();

		void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

		void ReComputeSize_PostChildren() override;

		void Draw(const DirectX::XMFLOAT2& windowSize) override;

		bool Handle(const Message& msg) override;

		void SetDirection(Direction dir);
		void SetSpace(const DirectX::XMINT2& space);

		LayoutStyle& GetDefaultStyle();

	private:
		Direction m_dir;

		DirectX::XMUINT2 m_previousMousePosition;

		DirectX::XMINT2 m_space; //space to add between widgets

		LayoutStyle m_defaultStyle;
	};
}
