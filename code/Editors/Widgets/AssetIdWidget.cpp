/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/AssetIdWidget.h"

#include "Systems/Assets/AssetId.h"

namespace Editors
{
	AssetIdWidget::AssetIdWidget(int32_t locX, int32_t locY, float scale, const Systems::AssetId& id)
		: Label()
	{
		SetX(locX);
		SetY(locY);
		SetFontScale(scale);
		SetText(id.ToString());
	}

	AssetIdWidget::~AssetIdWidget()
	{}
}
