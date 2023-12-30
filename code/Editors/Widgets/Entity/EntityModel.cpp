/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Entity/EntityModel.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"

#include "Editors/Widgets/Component/ComponentModel.h"

namespace Editors
{
	std::string EntityModel::s_default = "Unknown Value";

	EntityModel::EntityModel(Entity* pEntity)
		: m_pEntity(pEntity)
	{
		if (!pEntity)
			return;

		int count = m_pEntity->GetComponentCount();
		m_cachedModelArray.reserve(count);
		for (int ii = 0; ii < count; ++ii)
		{
			Component* pComponent = m_pEntity->GetComponent(ii);
			ComponentModel* pNewModel = new ComponentModel(pComponent);
			m_cachedModelArray.push_back(pNewModel);
		}
	}

	EntityModel::~EntityModel()
	{
		m_cachedModelArray.clear();
	}

	int EntityModel::GetComponentCount() const
	{
		if (!m_pEntity)
			return 0;

		return m_pEntity->GetComponentCount();
	}

	const std::string& EntityModel::GetComponentName(int row) const
	{
		if (!m_pEntity)
			return s_default;
	
		const Component* pComponent = m_pEntity->GetComponent(row);
		assert(pComponent);

		return pComponent->GetName();
	}

	ComponentModel* EntityModel::GetComponentModel(int row) const
	{
		if (!m_pEntity)
			return nullptr;

		assert(row >= 0 && row < GetComponentCount());
		return m_cachedModelArray[row];
	}
}
