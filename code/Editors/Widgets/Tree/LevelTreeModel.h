/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/Widgets/BaseModel.h"

#include <vector>

namespace Editors
{
	class Node;

	//Represent a node in the level tree. The data is the name of the node. Children are subrows.
	class LevelTreeModel : public BaseModel
	{
	public:
		LevelTreeModel(Node* pRoot);
		~LevelTreeModel();

		int GetRowCount() const override;
		int GetColumnCount() const override;
		int GetSubRowCount(int rowId) const override;

		const std::string& GetData(int rowId = 0, int columnId = 0) const override;

		BaseModel* GetSubModel(int rowId = 0, int columnId = 0) override;

		Node* GetSource();

	private:
		Node* m_pRoot;

		static std::string s_default;

		std::vector<LevelTreeModel*> m_subModels;
	};
}
