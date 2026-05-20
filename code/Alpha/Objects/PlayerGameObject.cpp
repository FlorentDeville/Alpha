/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/PlayerGameObject.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Commands/GameCommands.h"
#include "Alpha/Objects/BossGameObject.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Camera/CameraSubsystem.h"
#include "Systems/Game/Subsystems/Message/GameMessage.h"

#include <cmath>

PlayerGameObject::PlayerGameObject()
	: Systems::GameObject()
	, m_currentHp()
	, m_maxHp()
	, m_speed()
	, m_counterBulletIndex(UINT32_MAX)
{
	m_pCamera = new Rendering::Camera();
}

PlayerGameObject::~PlayerGameObject()
{
	delete m_pCamera;
}

void PlayerGameObject::OnStartGame()
{
	BaseClass::OnStartGame();

	Systems::TransformComponent& transform = GetTransform();
	const Core::Mat44f& localTx = transform.GetLocalTx();

	Systems::GameContext* pContext = Systems::GameMgr::Get().GetWorld();
	pContext->m_pCameraSubsystem->PushCamera(m_pCamera);
	m_pCamera->SetLookAt(localTx.GetT() + m_cameraOffset, localTx.GetT(), Core::Vec4f(0, 1, 0, 0));
	m_pCamera->SetProjection(45 * Core::PI_OVER_180, 1920.f / 1080.f, 0.1f, 1000);

	m_currentHp = m_maxHp;
}

void PlayerGameObject::Update(float dt)
{
	Systems::TransformComponent& transform = GetTransform();
	const Core::Mat44f& localTx = transform.GetLocalTx();

	const Core::Vec4f forward = localTx.GetZ();
	const Core::Vec4f right = localTx.GetX();
	const Core::Vec4f oldPosition = localTx.GetT();

	Core::Vec4f direction;
	if (GameCommands::MoveUp())
		direction = direction + forward;
	else if (GameCommands::MoveDown())
		direction = direction - forward;

	if (GameCommands::MoveLeft())
		direction = direction - right;
	else if (GameCommands::MoveRight())
		direction = direction + right;

	if (direction.Length() != 0)
	{
		direction.Normalize();
		Core::Vec4f newPosition = oldPosition + direction * m_speed * dt;

		Core::Mat44f newLocalTx = localTx;
		newLocalTx.SetRow(3, newPosition);

		transform.SetLocalTx(newLocalTx);
	}

	if (GameCommands::Counter())
	{
		//Core::LogModule::Get().LogInfo("Command Counter triggered");

		if(m_counterBulletIndex != UINT32_MAX)
		{
			BulletSubsystem* bulletSubsystem = BulletSubsystem::GetSubsystem();
			bulletSubsystem->CounteredBullet(m_counterBulletIndex);
		}
	}

	BaseClass::Update(dt);

	//cleanup
	m_counterBulletIndex = UINT32_MAX;
}

void PlayerGameObject::PostUpdate()
{
	UpdateCamera();
}

void PlayerGameObject::HandleMessage(const Systems::GameMessage& msg)
{
	switch (msg.m_id)
	{
	case SID("bullet_collision"):
	{
		OnBulletCollision();
	}
	break;

	case SID("counter_bullet_collision"):
	{
		Core::LogModule::Get().LogInfo("Message counter_bullet_collision received");
		m_counterBulletIndex = static_cast<uint32_t>(msg.m_param);
	}
	break;

	default:
		break;
	}
}

void PlayerGameObject::OnDestroyGame()
{
	BaseClass::OnDestroyGame();

	Systems::GameContext* pContext = Systems::GameMgr::Get().GetWorld();
	pContext->m_pCameraSubsystem->PopCamera();
}

void PlayerGameObject::OnBulletCollision()
{
	const uint32_t DAMAGE = 5;
	m_currentHp -= DAMAGE;
	if (m_currentHp > m_maxHp) m_currentHp = 0;

	Systems::UIBaseComponent* pCurrentHpUIComp = m_currentHealthComp.FindComponent(this);
	Systems::UIBaseComponent* pTotalHpUIComp = m_totalHealthComp.FindComponent(this);
	
	float totalWidth = pTotalHpUIComp->GetSize().x;
	float currentWidth = totalWidth / m_maxHp * m_currentHp;

	Core::Float2 size = pCurrentHpUIComp->GetSize();
	size.x = currentWidth;
	pCurrentHpUIComp->SetSize(size);

	Core::Float2 position = pCurrentHpUIComp->GetPosition();
	position.x = pTotalHpUIComp->GetPosition().x - ((totalWidth - currentWidth) / 2);
	pCurrentHpUIComp->SetPosition(position);
}

void PlayerGameObject::UpdateCamera()
{
	const BossGameObject* pBoss = Systems::GameMgr::Get().FindGameObject<BossGameObject>();
	Core::Vec4f bossPos = pBoss->GetTransform().GetWorldTx().GetT();

	const Core::Vec4f& playerPos = GetTransform().GetWorldTx().GetT();

	//calculate the midpoint
	Core::Vec4f targetPos = (bossPos + playerPos) * 0.5f;

	//calculate the distance
	float halfDistance = (bossPos - playerPos).Length() * 0.5f;

	//calculate distance using the vertical axis
	float phi = m_pCamera->GetFOV() / m_pCamera->GetAspectRatio();
	float halfPhi = phi * 0.5f;
	float cameraDistance = halfDistance / std::tan(halfPhi);

	//clamp
	const float MIN_DISTANCE = 70;
	const float MAX_DISTANCE = 100;
	cameraDistance = cameraDistance < MIN_DISTANCE ? MIN_DISTANCE : (cameraDistance > MAX_DISTANCE ? MAX_DISTANCE : cameraDistance);

	//compute camera position
	const float MARGIN = 5.f;
	m_cameraOffset.Normalize();
	Core::Vec4f cameraPosition = targetPos + m_cameraOffset * (cameraDistance + MARGIN);

	m_pCamera->SetLookAt(cameraPosition, targetPos, Core::Vec4f(0, 1, 0, 0));
}
