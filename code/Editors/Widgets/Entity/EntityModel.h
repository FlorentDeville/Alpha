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
		EntityModel(const Entity* pEntity);
		~EntityModel();

		int GetRowCount() const override;
		int GetColumnCount() const override;

		const std::string& GetData(int rowId = 0, int columnId = 0) const override;

		const BaseModel* GetSubModel(int rowId = 0, int columnId = 0) const override;

	private:
		std::vector<const BaseModel*> m_subModels;

		const Entity* m_pEntity;

		static std::string s_default;
	};
}
