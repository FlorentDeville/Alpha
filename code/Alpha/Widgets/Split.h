/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Widget.h"

namespace Widgets
{
	class Split : public Widget
	{
	public:
		Split();
		~Split();

		void Draw() override;
		//bool Handle(const Message& msg) override;

		//bool IsDragged() const;

	private:
		//bool m_isDragged;
	};
}
