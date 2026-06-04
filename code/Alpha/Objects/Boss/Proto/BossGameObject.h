/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Objects/Boss/BaseBoss.h"

#include "Core/Reflection/ReflectionMacro.h"

#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/GameComponent/Collisions/CollisionSphereComponent.h"
#include "Systems/GameComponent/ComponentRef/ComponentRef.h"
#include "Systems/GameComponent/UI/UIBaseComponent.h"

class StateMachine;

ENABLE_GAME_REFLECTION(BossGameObject)

class BossState_Phase1_Attack;
class BossState_Phase2_Attack1;
class BossState_Phase2_Attack2;
class BossState_Phase3_Attack1;
class BossState_Phase3_Attack2;

class BossGameObject : public BaseBoss
{
	using BaseClass = BaseBoss;

public:
	BossGameObject();
	~BossGameObject();

	void OnStartGame() override;

	void Update(float dt) override;

	void HandleMessage(const Systems::GameMessage& msg) override;

	void OnDestroyGame() override;

	void SetCurrentHP(int32_t hp) override;

	void EnterPhase1();
	void ExitPhase1();

	void EnterPhase2();
	void ExitPhase2();

	void EnterPhase3();

private:
	Systems::HardAssetRef<Systems::MeshAsset> m_mesh;
	Systems::HardAssetRef<Systems::MaterialInstanceAsset> m_material;
	Systems::HardAssetRef<Systems::MaterialInstanceAsset> m_counterBulletMaterial;

	Systems::ComponentRef<Systems::UIBaseComponent> m_currentHealthComp;
	Systems::ComponentRef<Systems::UIBaseComponent> m_totalHealthComp;
	Systems::ComponentRef<Systems::CollisionSphereComponent> m_collComp;

	START_REFLECTION(BossGameObject)
		ADD_BASETYPE(BaseBoss)
		ADD_FIELD(m_mesh)
		ADD_FIELD(m_material)
		ADD_FIELD(m_counterBulletMaterial)
		ADD_FIELD(m_currentHealthComp)
		ADD_FIELD(m_totalHealthComp)
		ADD_FIELD(m_collComp)
	END_REFLECTION()

private:
	BossState_Phase1_Attack* m_pStatePhase1Attack;
	BossState_Phase2_Attack1* m_pStatePhase2Attack1;
	BossState_Phase2_Attack2* m_pStatePhase2Attack2;
	BossState_Phase3_Attack1* m_pStatePhase3Attack1;
	BossState_Phase3_Attack2* m_pStatePhase3Attack2;

	void OnCollision(const Systems::ICollisionShape* pOther);

	void UpdateHPBar();
};
