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
		: BaseModel()
		, m_pEntity(pEntity)
	{
		if (!pEntity)
			return;

		int count = m_pEntity->GetComponentCount();
		m_subModels.reserve(count);
		for (int ii = 0; ii < count; ++ii)
		{
			Component* pComponent = m_pEntity->GetComponent(ii);
			ComponentModel* pNewModel = new ComponentModel(pComponent);
			m_subModels.push_back(pNewModel);
		}
	}

	EntityModel::~EntityModel()
	{
		for (const BaseModel* pModel : m_subModels)
			delete pModel;

		m_subModels.clear();
	}

	int EntityModel::GetRowCount() const
	{
		if (!m_pEntity)
			return 0;

		return m_pEntity->GetComponentCount();
	}

	int EntityModel::GetColumnCount() const
	{
		return 2; //0 : component name, 1 : component
	}

	const std::string& EntityModel::GetData(int rowId, int columnId) const
	{
		if (!m_pEntity)
			return s_default;

		assert(columnId == 0);
	
		const Component* pComponent = m_pEntity->GetComponent(rowId);
		assert(pComponent);

		return pComponent->GetName();
	}

	BaseModel* EntityModel::GetSubModel(int rowId, int columnId)
	{
		if (!m_pEntity)
			return nullptr;

		assert(columnId == 1);
		return m_subModels[rowId];
	}
}
