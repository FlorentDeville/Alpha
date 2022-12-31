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
	class BaseModel;

	class AssetIdWidget : public Widgets::Label
	{
	public:
		AssetIdWidget(int32_t locX, int32_t locY, float scale);
		~AssetIdWidget();

		void SetModel(const BaseModel* pModel);

		void Update(uint64_t dt) override;

	private:
		const BaseModel* m_pModel;
	};
}
