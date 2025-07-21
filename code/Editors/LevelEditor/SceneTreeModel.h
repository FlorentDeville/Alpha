/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Models/AbstractViewModel.h"
#include "Systems/Assets/AssetObjects/Level/LevelAsset.h"

namespace Systems
{
	class GameObject;
}

namespace Editors
{
	class SceneTreeModel : public Widgets::AbstractViewModel
	{
	public:
		enum Columns
		{
			Name = 0,
			Type,
			Guid,
			Modified,

			Count
		};

		SceneTreeModel(const Systems::LevelAsset* pLevel);
		~SceneTreeModel();

		// Override base class functions
		Widgets::ModelIndex GetParent(const Widgets::ModelIndex& index) const override;
		Widgets::ModelIndex GetIndex(int row, int column, const Widgets::ModelIndex& parent) const override;
		int GetRowCount(const Widgets::ModelIndex& parent) const override;
		int GetColumnCount(const Widgets::ModelIndex& parent) const override;
		std::string GetData(const Widgets::ModelIndex& index) override;
		std::string GetHeaderData(int column) override;

		// Specific functions
		void AddGameObject(const Systems::GameObject* pGo, const Systems::GameObject* pGoParent = nullptr);
		void RemoveGameObject(const Core::Guid& guid);
		void RenameGameObject(const Core::Guid& guid, const std::string& newName);

		Core::Guid FindGameObject(const Widgets::ModelIndex& index) const;

		void SelectGameObject(const Core::Guid& guid);

	private:

		struct CachedItem
		{
			Core::Guid m_guid;
			std::string m_name; //maybe I shouldn't cache this and query it?
			std::string m_type;
			Core::Guid m_parent;
			Core::Array<Core::Guid> m_children;
		};

		Core::Array<CachedItem*> m_cachedItemArray;			// flat list containing all the cached item
		std::map<Core::Guid, CachedItem*> m_cachedItemMap;	// map from a game object guid to the cached item
		Core::Array<CachedItem*> m_cachedItemRootsArray;	// list of root game objects.

		const CachedItem* CreateCachedItem(const Systems::GameObject* pGo);

		CachedItem* FindCachedItem(const Core::Guid& guid);
		const CachedItem* FindCachedItem(const Core::Guid& guid) const;

		Widgets::ModelIndex GetModelIndex(const CachedItem* pItem) const;
	};
}
