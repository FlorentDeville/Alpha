/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Editors/LevelEditor/Level.h"

#include <map>

namespace Rendering
{
	class MaterialId;
	class MeshId;
}

namespace Systems
{
	class AssetId;
}

namespace Widgets
{
	class Widget;
}

namespace Editors
{
	class Node;
	class GizmoWidget;
	
	class LevelEditor : public Core::Singleton<LevelEditor>
	{
	public:
		LevelEditor();
		~LevelEditor();

		void CreateEditor(Widgets::Widget* pParent);

		const Level& GetConstLevel() const;
		Level& GetLevel();

		void AddNewEntity(const std::string& name);
		void DeleteEntity(Node* pNode);

		//temp
		std::map<Systems::AssetId, Rendering::MeshId> m_assetIdToMeshId;
		std::map<Systems::AssetId, Rendering::MaterialId> m_assetIdToMaterialId;

	private:
		Level m_level;
	};
}
