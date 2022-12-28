/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Tree/LevelTreeModel.h"

#include "Editors/LevelEditor/Entity.h"

#include "Core/Tree/Tree.h"

namespace Editors
{
	std::string LevelTreeModel::s_default = "Unknown";

	LevelTreeModel::LevelTreeModel(const Core::TreeNode<Entity*>* pRoot)
		: BaseModel()
		, m_pRoot(pRoot)
	{
		const std::vector<Core::TreeNode<Entity*>*>& children = pRoot->GetChildren();
		for (const Core::TreeNode<Entity*>* pNode : children)
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
		//return static_cast<int>(m_pRoot->GetChildren().size());
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
		Entity* pNode = m_pRoot->GetContent();
		if (pNode)
			return pNode->GetName();

		return s_default;
	}

	const BaseModel* LevelTreeModel::GetSubModel(int rowId, int columnId) const
	{
		return m_subModels[rowId];
	}
}
