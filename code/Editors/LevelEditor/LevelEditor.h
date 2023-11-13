/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Editors/LevelEditor/Level.h"

#include "Rendering/Material/MaterialId.h"
#include "Rendering/Mesh/MeshId.h"

#include "Systems/Assets/AssetId.h"

#include <map>
#include <DirectXMath.h>

namespace Rendering
{
	class RenderTarget;
}

namespace Widgets
{
	class Label;
	class SplitVertical;
	class Widget;
}

namespace Editors
{
	class BaseModel;
	class EntityModel;
	class EntityWidget;
	class GizmoWidget;
	class LevelTreeModel;
	class Node;
	class TreeWidget;

	class LevelEditor : public Core::Singleton<LevelEditor>
	{
	public:
		LevelEditor();
		~LevelEditor();

		void CreateEditor(Widgets::Widget* pParent);

		void Update();
		void Render();

		const Level& GetConstLevel() const;
		Level& GetLevel();

		//temp
		std::map<Systems::AssetId, Rendering::MeshId> m_assetIdToMeshId;
		std::map<Systems::AssetId, Rendering::MaterialId> m_assetIdToMaterialId;

	private:
		Level m_level;

		//entity viewer
		EntityModel* m_pEntityModel;
		EntityWidget* m_pEntityWidget;
		TreeWidget* m_pTreeWidget;
		LevelTreeModel* m_pLevelTreeModel;

		Widgets::Label* m_pEntityNameLabel;


		GizmoWidget* m_pGizmoWidget;

		void CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit);
		void CreateSceneTreeViewer(Widgets::SplitVertical* pSplit);

		bool OnTreeItemClicked(BaseModel* pModel, int rowId);
		bool OnAddEntityClicked();
		bool OnRemoveEntityClicked();

		void AddNewEntity(const std::string& name);
		void DeleteEntity(Node* pNode);
	};
}