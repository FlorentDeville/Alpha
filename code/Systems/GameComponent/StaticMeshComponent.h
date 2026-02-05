/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"

#include "Systems/Assets/AssetRef/HardAssetRef.h"
#include "Systems/Objects/GameComponent.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, StaticMeshComponent)
	class StaticMeshComponent : public GameComponent
	{
	public:
		StaticMeshComponent();
		~StaticMeshComponent();

		void Render() override;

	private:
		HardAssetRef<MeshAsset> m_mesh;
		HardAssetRef<MaterialInstanceAsset> m_material;

		START_REFLECTION(Systems::StaticMeshComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_mesh)
			ADD_FIELD(m_material)
		END_REFLECTION()
	};
}
