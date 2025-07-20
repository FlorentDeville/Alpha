/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackId.h"
#include "Core/Singleton.h"

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
	class SplitVertical;
	class TabContainer;
	class TreeView;
	class Widget;
}

namespace Editors
{
	class EntityModel;
	class EntityWidget;
	class LevelEditorViewportWidget;
	class LevelListModel;
	class SceneTreeModel;

	class LevelEditor : public Core::Singleton<LevelEditor>
	{
	public:
		LevelEditor();
		~LevelEditor();

		void CreateEditor(Widgets::Widget* pParent);

	private:
		bool m_enableViewportControl;

		Widgets::Label* m_pEntityNameLabel;
		EntityWidget* m_pEntityWidget;
		LevelEditorViewportWidget* m_pViewport;
		Widgets::SplitVertical* m_pSplit; //split the right and center
		Widgets::SplitVertical* m_pLeftSplit; //split between the left and center
		Widgets::Frame* m_pSceneTreeFrame;

		Widgets::MenuItem* m_pSnapItem;

		Widgets::TabContainer* m_pLeftTabContainer;

		LevelListModel* m_pLevelListModel;
		EntityModel* m_pEntityModel;

		Widgets::TreeView* m_pSceneTree;
		SceneTreeModel* m_pSceneTreeModel;

		Core::CallbackId m_cidOnSelectionCleared_EntityProperties;

		Systems::NewAssetId m_selectedLevelInLevelList;

		void CreateMenuFile(Widgets::MenuBar* pMenuBar);
		void CreateMenuEdit(Widgets::MenuBar* pMenuBar);
		void CreateMenuTransformation(Widgets::MenuBar* pMenuBar);
		void CreateMenuWindows(Widgets::MenuBar* pMenuBar);

		void CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit);
		void CreateSceneTreeViewer(Widgets::TabContainer* pParent);
		void CreateLevelBrowser(Widgets::TabContainer* pParent);

		void CreateRenameModalWindow(const std::function<void(const std::string& newName)>& callback) const;

		void OnClick_SetGizmoModeSelection();
		void OnClick_SetGizmoModeTranslate();
		void OnClick_SetGizmoModeRotation();

		void OnSelectionCleared_EntityProperties();
		void OnAddedToSelection_EntityProperties(const Core::Guid& nodeGuid);
		void OnRemovedFromSelection_EntityProperties(const Core::Guid& nodeGuid);
		void OnRenameEntity_EntityProperties(const Core::Guid& nodeGuid);

		void OnSelectionCleared_Gizmo();
		void OnAddedToSelection_Gizmo(const Core::Guid& nodeGuid);
		void OnRemovedFromSelection_Gizmo(const Core::Guid& nodeGuid);

		void OnClickFileMenu_NewLevel();
		void OnClickFileMenu_OpenLevel();
		void OnClickFileMenu_SaveLevel();
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
		void OnLevelEditorModule_AddGameObject(const Systems::GameObject* pGo);
		void OnLevelEditorModule_RenameGameObject(const Core::Guid& guid, const std::string& newName);
		void OnLevelEditorModule_DeleteGameObject(const Core::Guid& guid);
	};
}
