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

class Widget;
namespace Rendering
{
	class RenderTarget;
}

namespace Editors
{
	class LevelEditor : public Core::Singleton<LevelEditor>
	{
	public:
		LevelEditor();
		~LevelEditor();

		void CreateEditor(Widget* pParent);

		void Update();
		void Render();

	private:
		Rendering::RenderTarget* m_pRenderTarget;
		float m_aspectRatio;

		Level m_level;

		//temp
		std::map<Systems::AssetId, Rendering::MeshId> m_assetIdToMeshId;
		std::map<Systems::AssetId, Rendering::MaterialId> m_assetIdToMaterialId;
	};
}