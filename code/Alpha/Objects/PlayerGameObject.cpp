/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/PlayerGameObject.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Commands/GameCommands.h"

#include "Core/Log/LogModule.h"
#include "Core/Math/Constants.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"

#include "Systems/Game/GameContext.h"
#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Camera/CameraSubsystem.h"
#include "Systems/Game/Subsystems/Message/GameMessage.h"

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

		m_pCamera->SetLookAt(localTx.GetT() + m_cameraOffset, localTx.GetT(), Core::Vec4f(0, 1, 0, 0));
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
