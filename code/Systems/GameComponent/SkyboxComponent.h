/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

//#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
//#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"

#include "Systems/Assets/AssetRef/HardAssetRef.h"
#include "Systems/Objects/GameComponent.h"

ENABLE_REFLECTION(Systems, SkyboxComponent)

namespace Systems
{
	class MeshAsset;
	class MaterialInstanceAsset;

	class SkyboxComponent : public GameComponent
	{
	public:
		SkyboxComponent();
		~SkyboxComponent();

		const MeshAsset* GetMesh() const;
		const MaterialInstanceAsset* GetMaterialInstance() const;

	private:
		HardAssetRef<MeshAsset> m_mesh;
		HardAssetRef<MaterialInstanceAsset> m_material;

		START_REFLECTION(Systems::SkyboxComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_mesh)
			ADD_FIELD(m_material)
		END_REFLECTION()
	};
}

