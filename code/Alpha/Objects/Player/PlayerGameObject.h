/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/ReflectionMacro.h"

#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/GameComponent/ComponentRef/ComponentRef.h"
#include "Systems/GameComponent/UI/UIBaseComponent.h"
#include "Systems/Objects/GameObject.h"

namespace Rendering
{
	class Camera;
}

class PlayerState_Move;
class PlayerWaveCountered;
class StateMachine;

ENABLE_GAME_REFLECTION(PlayerGameObject)

class PlayerGameObject : public Systems::GameObject
{
	using BaseClass = Systems::GameObject;

public:
	PlayerGameObject();
	~PlayerGameObject();

	void OnStartGame() override;

	void Update(float dt) override;
	void PostUpdate() override;

	void HandleMessage(const Systems::GameMessage& msg) override;

	void OnDestroyGame() override;

	float GetSpeed() const;

private:
	float m_speed;
	Core::Vec4f m_cameraOffset;

	uint32_t m_maxHp;

	Systems::ComponentRef<Systems::UIBaseComponent> m_currentHealthComp;
	Systems::ComponentRef<Systems::UIBaseComponent> m_totalHealthComp;

	Systems::HardAssetRef<Systems::MeshAsset> m_counteredBulletMesh;
	Systems::HardAssetRef<Systems::MaterialInstanceAsset> m_counteredBulletMaterial;

	START_REFLECTION(PlayerGameObject)
		ADD_BASETYPE(Systems::GameObject)
		ADD_FIELD(m_speed)
		ADD_FIELD(m_cameraOffset)
		ADD_FIELD(m_maxHp)
		ADD_FIELD(m_currentHealthComp)
		ADD_FIELD(m_totalHealthComp)
		ADD_FIELD(m_counteredBulletMesh)
		ADD_FIELD(m_counteredBulletMaterial)
	END_REFLECTION()

	Rendering::Camera* m_pCamera;

	uint32_t m_currentHp;

	StateMachine* m_pStateMachine;
	PlayerState_Move* m_pStateMove;

	PlayerWaveCountered* m_pCounteredBulletWave;
	uint32_t m_counteredBulletWaveIndex;

	void OnBulletCollision(uint32_t index);

	void UpdateCamera();
};
