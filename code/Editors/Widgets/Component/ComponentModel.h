/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/BaseModel.h"

#include <vector>

namespace Editors
{
	class Component;

	class ComponentModel : public BaseModel
	{
	public:
		ComponentModel(Component* pComponent);
		~ComponentModel();

		int GetRowCount() const override;
		int GetColumnCount() const override;

		const std::string& GetData(int rowId = 0, int columnId = 0) const override;
		PropertyType GetDataType(int rowId = 0, int columnId = 0) const override;

		BaseModel* GetSubModel(int rowId = 0, int columnId = 0) override;

	private:
		Component* m_pComponent;

		std::vector<BaseModel*> m_models;

		static std::string s_defaultValue;
	};
}
