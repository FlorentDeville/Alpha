/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

//#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
//#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
//#include "Systems/GameComponent/Collisions/CollisionSphereComponent.h"
//#include "Systems/GameComponent/ComponentRef/ComponentRef.h"
//#include "Systems/GameComponent/UI/UIBaseComponent.h"
#include "Systems/Objects/GameObject.h"

class StateMachine;

ENABLE_GAME_REFLECTION(BaseBoss)

class BossState_Phase1_Attack;
class BossState_Phase2_Attack1;
class BossState_Phase2_Attack2;
class BossState_Phase3_Attack1;
class BossState_Phase3_Attack2;

class BaseBoss : public Systems::GameObject
{
	using BaseClass = Systems::GameObject;

public:
	BaseBoss();
	~BaseBoss();

	void OnStartGame() override;

	void Update(float dt) override;

	void HandleMessage(const Systems::GameMessage& msg) override;

	void OnDestroyGame() override;

	uint32_t GetMaxHP() const;
	int32_t GetCurrentHP() const;

	virtual void SetCurrentHP(int32_t hp);

protected:
	uint32_t m_maxHP;

	START_REFLECTION(BaseBoss)
		ADD_BASETYPE(Systems::GameObject)
		ADD_FIELD(m_maxHP)
	END_REFLECTION()

protected:
	StateMachine* m_pStateMachine;
	int32_t m_currentHP;
};
