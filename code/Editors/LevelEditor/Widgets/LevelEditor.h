/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackId.h"
#include "Core/Sid/Sid.h"

#include "Editors/BaseEditor.h"

#include "Systems/Assets/NewAssetId.h"

#include <functional>
#include <string>

namespace Core
{
	class Guid;
}

namespace Rendering
{
	class RenderTarget;
}

namespace Systems
{
	class AssetMetadata;
	class GameObject;
}

namespace Widgets
{
	class Frame;
	class Label;
	class MenuBar;
	class MenuItem;
	class ModelIndex;
	class SplitVertical;
	class TabContainer;
	class TableView;
	class TreeView;
	class Widget;
}

namespace Editors
{
	class EntityModel;
	class EntityWidget;
	class LevelEditorViewportWidget;
	class LevelListModel;
	class PropertyGridPopulator;
	class PropertyGridWidget;
	class SceneTreeModel;

	class LevelEditor : public BaseEditor
	{
	public:
		LevelEditor();
		~LevelEditor();

		void CreateEditor(Widgets::Widget* pParent) override;

	private:
		bool m_enableViewportControl;

		LevelEditorViewportWidget* m_pViewport;
		Widgets::SplitVertical* m_pSplit; //split the right and center
		Widgets::SplitVertical* m_pLeftSplit; //split between the left and center
		Widgets::Frame* m_pSceneTreeFrame;

		Widgets::MenuItem* m_pSnapItem;

		Widgets::TabContainer* m_pLeftTabContainer;

		Widgets::TableView* m_pLevelTableView;
		LevelListModel* m_pLevelListModel;

		Widgets::TreeView* m_pSceneTree;
		SceneTreeModel* m_pSceneTreeModel;

		PropertyGridWidget* m_pPropertyGridWidget;
		PropertyGridPopulator* m_pPropertyGridPopulator;

		Systems::NewAssetId m_selectedLevelInLevelList;

		Core::Sid m_sceneTreeTabId;
		Core::Sid m_levelBrowserTabId;

		void CreateMenuFile(Widgets::MenuBar* pMenuBar);
		void CreateMenuEdit(Widgets::MenuBar* pMenuBar);
		void CreateMenuTransformation(Widgets::MenuBar* pMenuBar);
		void CreateMenuWindows(Widgets::MenuBar* pMenuBar);

		void CreateGameObjectPropertyGrid(Widgets::SplitVertical* pSplit);
		void CreateSceneTreeViewer(Widgets::TabContainer* pParent);
		void CreateLevelBrowser(Widgets::TabContainer* pParent);

		void CreateRenameModalWindow(const std::function<void(const std::string& newName)>& callback) const;

		void OnClick_SetGizmoModeSelection();
		void OnClick_SetGizmoModeTranslate();
		void OnClick_SetGizmoModeRotation();

		void OnSelectionCleared_GameObjectProperties();
		void OnAddedToSelection_GameObjectProperties(const Core::Guid& guid);
		void OnRemovedFromSelection_GameObjectProperties(const Core::Guid& nodeGuid);

		void OnSelectionCleared_Gizmo();
		void OnAddedToSelection_Gizmo(const Core::Guid& nodeGuid);
		void OnRemovedFromSelection_Gizmo(const Core::Guid& nodeGuid);

		void OnSelectionCleared_SceneTree();
		void OnAddedToSelection_SceneTree(const Core::Guid& guid);
		void OnRemovedFromSelectionSceneTree(const Core::Guid& guid);

		void OnClickFileMenu_NewLevel();
		void OnClickFileMenu_OpenLevel();
		void OnClickFileMenu_SaveLevel();
		void OnClickFileMenu_CloseLevel();
		void OnClickFileMenu_DeleteLevel();
		void OnClickFileMenu_RenameLevel();

		void OnClickEditMenu_AddGameObject();
		void OnClickEditMenu_DeleteGameObject();
		void OnClickEditMenu_RenameGameObject();
		void OnClickEditMenu_DuplicateEntity();
		void OnClickEditMenu_ReparentGameObject();

		void OnClickTransformationMenu_Snap();

		void OnLevelEditorModule_BeforeDeleteLevel(const Systems::AssetMetadata& metadata);
		void OnLevelEditorModule_RenameLevel(Systems::NewAssetId id, const std::string& newName);
		void OnLevelEditorModule_OpenLevel();
		void OnLevelEditorModule_ClosedLevel(Systems::NewAssetId levelId);
		void OnLevelEditorModule_AddGameObject(const Systems::GameObject* pGo, const Systems::GameObject* pGoParent);
		void OnLevelEditorModule_DeleteGameObject(const Core::Guid& guid);
		void OnLevelEditorModule_ReparentGameObject(const Systems::GameObject* pGo, const Systems::GameObject* pGoOldParent, const Systems::GameObject* pGoNewParent);

		void OnSceneTreeView_OnDropItem(const Widgets::ModelIndex& index, const Widgets::ModelIndex& parent);

		bool IsLevelBrowserEnabled() const;
		bool IsSceneTreeEnabled() const;
	};
}
