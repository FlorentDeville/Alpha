/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Assets/AssetRef/HardAssetRef.h"
#include "Systems/Objects/GameObject.h"

ENABLE_REFLECTION(Systems, StaticMesh)

namespace Systems
{
	class MaterialInstanceAsset;
	class MeshAsset;

	class StaticMesh : public GameObject
	{
	public:
		StaticMesh() = default;
		~StaticMesh() = default;

		const MeshAsset* GetMesh() const;
		const MaterialInstanceAsset* GetMaterialInstance() const;

	private:
		HardAssetRef<MeshAsset> m_mesh;
		HardAssetRef<MaterialInstanceAsset> m_material;

		START_REFLECTION(Systems::StaticMesh)
			ADD_BASETYPE(Systems::GameObject)
			ADD_FIELD(m_mesh)
			ADD_FIELD(m_material)
		END_REFLECTION()
	};
}
