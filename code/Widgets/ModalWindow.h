/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Container.h"

namespace Widgets
{
	class ModalWindow : public Container
	{
	public:
		ModalWindow();
		~ModalWindow();

		void ReComputePosition(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize) override;

	private:

	};
}
