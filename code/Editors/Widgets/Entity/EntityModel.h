/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <vector>

namespace Editors
{
	class ComponentModel;
	class Entity;

	class EntityModel
	{
	public:
		EntityModel(Entity* pEntity);
		~EntityModel();

		int GetComponentCount() const;

		const std::string& GetComponentName(int row) const;

		ComponentModel* GetComponentModel(int row) const;

	private:
		Entity* m_pEntity; //we should have an id here instead of a pointer
		std::vector<ComponentModel*> m_cachedModelArray;

		static std::string s_default;
	};
}
