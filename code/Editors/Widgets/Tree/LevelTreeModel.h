/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/BaseModel.h"

#include <vector>

namespace Editors
{
	class Entity;
}

namespace Core
{
	template<class T = Editors::Entity*> class TreeNode;
}

namespace Editors
{
	//Represent a node in the level tree. The data is the name of the node. Children are subrows.
	class LevelTreeModel : public BaseModel
	{
	public:
		LevelTreeModel(Core::TreeNode<Entity*>* pRoot);
		~LevelTreeModel();

		int GetRowCount() const override;
		int GetColumnCount() const override;
		int GetSubRowCount(int rowId) const override;

		const std::string& GetData(int rowId = 0, int columnId = 0) const override;

		BaseModel* GetSubModel(int rowId = 0, int columnId = 0) override;

		Core::TreeNode<Entity*>* GetSource();

	private:
		Core::TreeNode<Entity*>* m_pRoot;

		static std::string s_default;

		std::vector<LevelTreeModel*> m_subModels;
	};
}
