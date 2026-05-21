/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/GameComponent/Collisions/CollisionSphereComponent.h"
#include "Systems/GameComponent/ComponentRef/ComponentRef.h"
#include "Systems/GameComponent/UI/UIBaseComponent.h"
#include "Systems/Objects/GameObject.h"

class StateMachine;

ENABLE_GAME_REFLECTION(BossGameObject)

class BossState_Phase1_Attack;
class BossState_Phase2_Attack1;
class BossState_Phase2_Attack2;

class BossGameObject : public Systems::GameObject
{
	using BaseClass = Systems::GameObject;

public:
	BossGameObject();
	~BossGameObject();

	void OnStartGame() override;

	void Update(float dt) override;

	void HandleMessage(const Systems::GameMessage& msg) override;

	void OnDestroyGame() override;

	uint32_t GetMaxHP() const;
	int32_t GetCurrentHP() const;

	void SetCurrentHP(int32_t hp);

	void EnterPhase1();
	void ExitPhase1();

	void EnterPhase2();

private:
	Systems::HardAssetRef<Systems::MeshAsset> m_mesh;
	Systems::HardAssetRef<Systems::MaterialInstanceAsset> m_material;
	Systems::HardAssetRef<Systems::MaterialInstanceAsset> m_counterBulletMaterial;

	uint32_t m_maxHP;

	Systems::ComponentRef<Systems::UIBaseComponent> m_currentHealthComp;
	Systems::ComponentRef<Systems::UIBaseComponent> m_totalHealthComp;
	Systems::ComponentRef<Systems::CollisionSphereComponent> m_collComp;

	START_REFLECTION(BossGameObject)
		ADD_BASETYPE(Systems::GameObject)
		ADD_FIELD(m_mesh)
		ADD_FIELD(m_material)
		ADD_FIELD(m_counterBulletMaterial)
		ADD_FIELD(m_maxHP)
		ADD_FIELD(m_currentHealthComp)
		ADD_FIELD(m_totalHealthComp)
		ADD_FIELD(m_collComp)
	END_REFLECTION()

private:
	StateMachine* m_pStateMachine;
	int32_t m_currentHP;

	BossState_Phase1_Attack* m_pStatePhase1Attack;
	BossState_Phase2_Attack1* m_pStatePhase2Attack1;
	BossState_Phase2_Attack2* m_pStatePhase2Attack2;

	void OnCollision(const Systems::ICollisionShape* pOther);

	void UpdateHPBar();
};
