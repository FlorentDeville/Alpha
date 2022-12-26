/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Label.h"

namespace Systems
{
	class AssetId;
}

namespace Editors
{
	class AssetIdWidget : public Widgets::Label
	{
	public:
		AssetIdWidget(int32_t locX, int32_t locY, float scale, const Systems::AssetId& id);
		~AssetIdWidget();
	};
}
