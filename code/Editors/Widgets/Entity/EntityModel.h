/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/BaseModel.h"

#include <vector>

namespace Editors
{
	class Entity;

	class EntityModel : public BaseModel
	{
	public:
		EntityModel(Entity* pEntity);
		~EntityModel();

		int GetRowCount() const override;
		int GetColumnCount() const override;

		const std::string& GetData(int rowId = 0, int columnId = 0) const override;

		BaseModel* GetSubModel(int rowId = 0, int columnId = 0) override;

	private:
		std::vector<BaseModel*> m_subModels;

		Entity* m_pEntity;

		static std::string s_default;
	};
}
