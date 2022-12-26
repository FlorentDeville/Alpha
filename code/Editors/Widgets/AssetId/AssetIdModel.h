/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/BaseModel.h"
#include "Systems/Assets/AssetId.h"

namespace Editors
{
	class AssetIdModel : public BaseModel
	{
	public:
		AssetIdModel(Systems::AssetId id);
		~AssetIdModel();

		const std::string& GetData() const;

	private:
		Systems::AssetId m_id;

		mutable std::string m_cachedValue;
	};
}
