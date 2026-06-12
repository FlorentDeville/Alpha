/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"

#include "Systems/Assets/AssetRef/HardAssetRef.h"
#include "Systems/Objects/GameComponent.h"

ENABLE_REFLECTION(Systems, RenderableComponent)

namespace Systems
{
	// Component to render a mesh. The mesh and material can be changed at runtime.
	class RenderableComponent : public GameComponent
	{
	public:
		RenderableComponent();
		~RenderableComponent();

		void OnStart(GameContext* pWorld) override;
		void OnDestroy(GameContext* pWorld) override;

		void SetMesh(MeshAsset* pMesh);
		const MeshAsset* GetMesh() const;

		const Core::Sqt& GetLocalTx() const;

		void SetMaterialInstance(MaterialInstanceAsset*);
		const MaterialInstanceAsset* GetMaterialInstance() const;

	private:
		//default mesh and materials that can be set in the editor
		HardAssetRef<MeshAsset> m_mesh;
		HardAssetRef<MaterialInstanceAsset> m_material;

		Core::Sqt m_lsTx;

		START_REFLECTION(Systems::RenderableComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_mesh)
			ADD_FIELD(m_material)
			ADD_FIELD(m_lsTx)
		END_REFLECTION()

		//runtime pointers
		MeshAsset* m_pMesh;
		MaterialInstanceAsset* m_pMaterial;
	};
}
