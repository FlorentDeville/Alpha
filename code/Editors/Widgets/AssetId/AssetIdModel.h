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
		AssetIdModel(Systems::AssetId& id, PropertyType type);
		~AssetIdModel();

		int GetRowCount() const override;
		int GetColumnCount() const override;

		const std::string& GetData(int rowId=0, int columnId=0) const override;

		PropertyType GetDataType(int rowId = 0, int columnId = 0) const override;

		void SetData(int rowId, int columnId, const std::string& value) override;

	private:
		Systems::AssetId& m_id;
		PropertyType m_type;

		mutable std::string m_cachedValue;
	};
}
