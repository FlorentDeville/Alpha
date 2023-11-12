/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Tree/LevelTreeModel.h"

#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/Node.h"

#include "Core/Tree/Tree.h"

namespace Editors
{
	std::string LevelTreeModel::s_default = "Unknown";

	LevelTreeModel::LevelTreeModel(Node* pRoot)
		: BaseModel()
		, m_pRoot(pRoot)
	{
		const std::vector<Node*>& children = pRoot->GetChildren();
		for (Node* pNode : children)
			m_subModels.push_back(new LevelTreeModel(pNode));
	}

	LevelTreeModel::~LevelTreeModel()
	{
		for (LevelTreeModel* pModel : m_subModels)
			delete pModel;

		m_subModels.clear();
	}

	int LevelTreeModel::GetRowCount() const
	{
		return 1;
	}

	int LevelTreeModel::GetColumnCount() const
	{
		return 1;
	}

	int LevelTreeModel::GetSubRowCount(int rowId) const
	{
		return static_cast<int>(m_pRoot->GetChildren().size());
	}

	const std::string& LevelTreeModel::GetData(int rowId, int columnId) const
	{
		const Entity* pEntity = m_pRoot->ToConstEntity();
		if (pEntity)
			return pEntity->GetName();

		return s_default;
	}

	BaseModel* LevelTreeModel::GetSubModel(int rowId, int columnId)
	{
		return m_subModels[rowId];
	}

	Node* LevelTreeModel::GetSource()
	{
		return m_pRoot;
	}
}
