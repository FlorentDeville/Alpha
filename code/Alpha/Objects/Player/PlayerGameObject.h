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

namespace Systems
{
	class MeshComponent;
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

	void SpawnCounteredBullet(const Core::Vec4f& startPosition);

	void SetCameraModeTracking();
	void SetCameraModeLocked();

	void ShowDashCircle();
	void HideDashCircle();

	void ShowDashTarget();
	void HideDashTarget();

	//Position relative to the player
	void SetDashTargetRelativePosition(const Core::Vec4f& relPosition);

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

	enum CameraMode
	{
		TRACK,	//keep player and boss in the view
		LOCK,	//lock in place

		COUNT
	};

	CameraMode m_cameraMode;

	Systems::GameObject* m_pDashCircleObject;
	Systems::MeshComponent* m_pDashCircleMesh;

	Systems::GameObject* m_pDashTargetObject;
	Systems::MeshComponent* m_pDashTargetMesh;

	void OnBulletCollision(uint32_t index);

	void UpdateCamera();

	void OnStartGame_DashCircle();
	void OnGameStart_DashTarget();
};
