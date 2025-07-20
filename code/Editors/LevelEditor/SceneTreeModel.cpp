/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/SceneTreeModel.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"

#include "Widgets/Models/ModelIndex.h"

namespace Editors
{
	SceneTreeModel::SceneTreeModel()
		: AbstractViewModel()
	{ }

	SceneTreeModel::~SceneTreeModel()
	{ }

	Widgets::ModelIndex SceneTreeModel::GetParent(const Widgets::ModelIndex&) const
	{
		return Widgets::ModelIndex(); //there is no parent in arrays
	}

	Widgets::ModelIndex SceneTreeModel::GetIndex(int row, int column, const Widgets::ModelIndex& parent) const
	{
		return Widgets::ModelIndex();
		//return CreateIndex(row, column, &data);
	}

	int SceneTreeModel::GetRowCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return 0;
	}

	int SceneTreeModel::GetColumnCount(const Widgets::ModelIndex& parent) const
	{
		if (parent.IsValid())
			return 0;

		return Columns::Count;
	}

	std::string SceneTreeModel::GetData(const Widgets::ModelIndex& index)
	{
		return "empty";
	}

	std::string SceneTreeModel::GetHeaderData(int column)
	{
		switch (column)
		{
		case Columns::Guid:
			return "Guid";
			break;

		case Columns::Name:
			return "Name";
			break;

		case Columns::Type:
			return "Type";
			break;

		case Columns::Modified:
			return "Modified";
			break;

		default:
			return "";
			break;
		}
	}
}
