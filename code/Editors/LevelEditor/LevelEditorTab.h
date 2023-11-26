/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Tab.h"

namespace Rendering
{
	class RenderTarget;
}

namespace Widgets
{
	class Frame;
	class Label;
	class MenuBar;
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

		EntityModel* m_pEntityModel;
		LevelTreeModel* m_pLevelTreeModel;

		void CreateMenuTransformation(Widgets::MenuBar* pMenuBar);
		void CreateMenuWindows(Widgets::MenuBar* pMenuBar);

		void CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit);
		void CreateSceneTreeViewer(Widgets::SplitVertical* pSplit);

		bool OnClick_AddEntity();
		bool OnClick_RemoveEntity();

		bool OnClick_TreeItem(BaseModel* pModel, int rowId);
	};
}
