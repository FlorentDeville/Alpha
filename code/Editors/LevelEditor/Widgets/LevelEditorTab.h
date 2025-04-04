/********************************************************************/
/* � 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Tab.h"

namespace Os
{
	class Guid;
}

namespace Rendering
{
	class RenderTarget;
}

namespace Widgets
{
	class Frame;
	class Label;
	class MenuBar;
	class MenuItem;
	class SplitVertical;
}

namespace Editors
{
	class BaseModel;
	class EntityModel;
	class EntityWidget;
	class LevelEditorViewportWidget;
	class LevelTreeModel;
	class TreeWidget;

	class LevelEditorTab : public Widgets::Tab
	{
	public:
		LevelEditorTab(Widgets::Widget* pParent = nullptr);
		virtual ~LevelEditorTab();

	private:
		bool m_enableViewportControl;

		Widgets::Label* m_pEntityNameLabel;
		EntityWidget* m_pEntityWidget;
		TreeWidget* m_pTreeWidget;
		LevelEditorViewportWidget* m_pViewport;
		Widgets::SplitVertical* m_pSplit; //split the right and center
		Widgets::SplitVertical* m_pLeftSplit; //split between the left and center
		Widgets::Frame* m_pSceneTreeFrame;

		Widgets::MenuItem* m_pSnapItem;

		EntityModel* m_pEntityModel;
		LevelTreeModel* m_pLevelTreeModel;

		Core::CallbackId m_cidOnSelectionCleared_EntityProperties;

		void CreateMenuFile(Widgets::MenuBar* pMenuBar);
		void CreateMenuEdit(Widgets::MenuBar* pMenuBar);
		void CreateMenuTransformation(Widgets::MenuBar* pMenuBar);
		void CreateMenuWindows(Widgets::MenuBar* pMenuBar);

		void CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit);
		void CreateSceneTreeViewer(Widgets::SplitVertical* pSplit);

		void CreateRenameModalWindow(const std::function<void(const std::string& newName)>& callback) const;

		bool OnClick_TreeItem(BaseModel* pModel, int rowId);

		void OnClick_SetGizmoModeSelection();
		void OnClick_SetGizmoModeTranslate();
		void OnClick_SetGizmoModeRotation();

		void OnSelectionCleared_EntityProperties();
		void OnAddedToSelection_EntityProperties(const Os::Guid& nodeGuid);
		void OnRemovedFromSelection_EntityProperties(const Os::Guid& nodeGuid);
		void OnRenameEntity_EntityProperties(const Os::Guid& nodeGuid);

		void OnSelectionCleared_Gizmo();
		void OnAddedToSelection_Gizmo(const Os::Guid& nodeGuid);
		void OnRemovedFromSelection_Gizmo(const Os::Guid& nodeGuid);

		void OnAddEntity_SceneTree(const Os::Guid& nodeGuid);
		void OnDeleteEntity_SceneTree(const Os::Guid& nodeGuid);
		void OnRenameEntity_SceneTree(const Os::Guid& nodeGuid);
		void OnDuplicateEntity_SceneTree(const Os::Guid& src, const Os::Guid& copy);
		void OnNewLevel_SceneTree();
		void OnLoadLevel_SceneTree();

		void OnClickFileMenu_LoadLevel();
		void OnClickFileMenu_Save();
		void OnClickFileMenu_SaveAs();

		void OnClickEditMenu_AddEntity();
		void OnClickEditMenu_DeleteEntity();
		void OnClickEditMenu_RenameEntity();
		void OnClickEditMenu_DuplicateEntity();

		void OnClickTransformationMenu_Snap();
	};
}
