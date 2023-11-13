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
	class Label;
	class SplitVertical;
}

namespace Editors
{
	class BaseModel;
	class EntityModel;
	class EntityWidget;
	class LevelTreeModel;
	class TreeWidget;

	class LevelEditorTab : public Widgets::Tab
	{
	public:
		LevelEditorTab(Widgets::Widget* pParent = nullptr);
		virtual ~LevelEditorTab();

	private:
		Rendering::RenderTarget* m_pRenderTarget;
		float m_aspectRatio;

		bool m_enableViewportControl;

		Widgets::Label* m_pEntityNameLabel;
		EntityWidget* m_pEntityWidget;
		TreeWidget* m_pTreeWidget;

		EntityModel* m_pEntityModel;
		LevelTreeModel* m_pLevelTreeModel;

		void CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit);
		void CreateSceneTreeViewer(Widgets::SplitVertical* pSplit);

		bool OnClick_AddEntity();
		bool OnClick_RemoveEntity();

		bool OnClick_TreeItem(BaseModel* pModel, int rowId);
	};
}
