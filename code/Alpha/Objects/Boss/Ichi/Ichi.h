/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Alpha/Objects/Boss/BaseBoss.h"
#include "Alpha/Objects/Boss/Ichi/Motion/IchiMotionState.h"

#include "Core/Reflection/ReflectionMacro.h"

#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Assets/AssetObjects/ParticleEffect/ParticleEffectAsset.h"
#include "Systems/Game/Subsystems/Particle/ParticleEffectHandle.h"
#include "Systems/GameComponent/Collisions/CollisionSphereComponent.h"
#include "Systems/GameComponent/ComponentRef/ComponentRef.h"
#include "Systems/GameComponent/RenderableComponent.h"
#include "Systems/GameComponent/UI/UIBaseComponent.h"

ENABLE_GAME_REFLECTION(Ichi)

class Ichi : public BaseBoss
{
	using BaseClass = BaseBoss;

public:
	Ichi();
	~Ichi();

	void OnStartGame() override;

	void Update(float dt) override;

	void HandleMessage(const Systems::GameMessage& msg) override;

	void OnDestroyGame() override;

	void SetCurrentHP(int32_t hp) override;

	void EnterPhase1();
	void ExitPhase1();

	void EnterPhase2();

	/*
	

	void EnterPhase2();
	void ExitPhase2();

	void EnterPhase3();*/

	void SpawnEngineEffects();
	void KillEngineEffects();

	void GoToMotionState(IchiMotionState::Type newState);
	void GoToMotionStateTravel(const Core::Vec4f& target);

	bool IsInMotionState(IchiMotionState::Type state) const;

	Systems::MeshAsset* GetBulletMesh();
	Systems::MaterialInstanceAsset* GetBulletMaterial();
	Systems::MaterialInstanceAsset* GetCounterableBulletMaterial();

	const uint8_t GetPhase1GunsAttachPointsCount() const;
	const Core::Mat44f* GetPhase1GunsAttachPoints() const;

private:
	Systems::HardAssetRef<Systems::MeshAsset> m_meshPhase1;
	Systems::HardAssetRef<Systems::MeshAsset> m_meshPhase2;
	Systems::HardAssetRef<Systems::MeshAsset> m_meshPhase3;
	
	Systems::HardAssetRef<Systems::MeshAsset> m_bulletMesh;
	Systems::HardAssetRef<Systems::MaterialInstanceAsset> m_bulletMaterial;
	Systems::HardAssetRef<Systems::MaterialInstanceAsset> m_counterBulletMaterial;

	Systems::HardAssetRef<Systems::ParticleEffectAsset> m_engineEffect;

	Systems::ComponentRef<Systems::UIBaseComponent> m_currentHealthComp;
	Systems::ComponentRef<Systems::UIBaseComponent> m_totalHealthComp;
	Systems::ComponentRef<Systems::CollisionSphereComponent> m_collComp;

	Systems::ComponentRef<Systems::RenderableComponent> m_phase1Renderable;
	Systems::ComponentRef<Systems::RenderableComponent> m_phase2Renderable;
	Systems::ComponentRef<Systems::RenderableComponent> m_phase3Renderable;

	START_REFLECTION(Ichi)
		ADD_BASETYPE(BaseBoss)
		ADD_FIELD(m_meshPhase1)
		ADD_FIELD(m_meshPhase2)
		ADD_FIELD(m_meshPhase3)
		ADD_FIELD(m_engineEffect)
		ADD_FIELD(m_bulletMesh)
		ADD_FIELD(m_bulletMaterial)
		ADD_FIELD(m_counterBulletMaterial)
		ADD_FIELD(m_currentHealthComp)
		ADD_FIELD(m_totalHealthComp)
		ADD_FIELD(m_collComp)
		ADD_FIELD(m_phase1Renderable)
		ADD_FIELD(m_phase2Renderable)
		ADD_FIELD(m_phase3Renderable)
	END_REFLECTION()

private:
	static const uint8_t ENGINE_EFFECT_COUNT = 4;
	Systems::ParticleEffectHandle m_engineEffectHandle[ENGINE_EFFECT_COUNT];

	static const uint8_t GUN_PHASE1_COUNT = 4;
	static Core::Sid ATTACH_POINT_PHASE1_GUN_NAME[GUN_PHASE1_COUNT];
	Core::Mat44f m_phase1GunsAttachPoints[GUN_PHASE1_COUNT];

	StateMachine* m_pMotionStateMachine;

	static Core::Sid ATTACH_POINTS_NAME[ENGINE_EFFECT_COUNT];
	Core::Mat44f m_engineAttachPoints[ENGINE_EFFECT_COUNT];
	//void OnCollision(const Systems::ICollisionShape* pOther);

	void UpdateHPBar();

	void SkipStart();
};
