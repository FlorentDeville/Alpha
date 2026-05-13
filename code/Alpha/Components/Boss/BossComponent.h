/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Systems/Objects/GameComponent.h"

#include "Systems/Assets/AssetRef/HardAssetRef.h"
#include "Systems/GameComponent/ComponentRef/ComponentRef.h"
#include "Systems/GameComponent/UI/UIBaseComponent.h"

ENABLE_GAME_REFLECTION(BossComponent)

namespace Systems
{
	class GameContext;
	class ICollisionShape;
	class MaterialInstanceAsset;
	class MeshAsset;
}

class StateMachine;

class BossComponent : public Systems::GameComponent
{
public:
	BossComponent();
	~BossComponent();

	void PostLoad() override;

	void OnStartGame() override;
	void Update(float dt) override;
	void OnDestroyGame() override;

private:

	Systems::HardAssetRef<Systems::MeshAsset> m_mesh;
	Systems::HardAssetRef<Systems::MaterialInstanceAsset> m_material;

	uint32_t m_maxHP;

	Systems::ComponentRef<Systems::UIBaseComponent> m_componentRef;

	START_REFLECTION(BossComponent)
		ADD_BASETYPE(Systems::GameComponent)
		ADD_FIELD(m_mesh)
		ADD_FIELD(m_material)
		ADD_FIELD(m_maxHP)
		ADD_FIELD(m_componentRef);
	END_REFLECTION()

	StateMachine* m_pStateMachine;
	uint32_t m_currentHP;

	void Move(float dt);

	void OnCollision(const Systems::ICollisionShape* pOther);
};
