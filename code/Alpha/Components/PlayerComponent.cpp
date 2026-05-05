/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/PlayerComponent.h"

#include "Core/Math/Constants.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"

#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/CameraSubsystem.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Objects/GameObject.h"

PlayerComponent::PlayerComponent()
	: GameComponent()
	, m_speed(1)
	, m_pCamera(nullptr)
{
	m_pCamera = new Rendering::Camera();
}

PlayerComponent::~PlayerComponent()
{
	delete m_pCamera;
}

void PlayerComponent::PostLoad()
{ }

void PlayerComponent::OnStart(Systems::World* pWorld)
{
	Systems::TransformComponent& transform = GetOwner()->GetTransform();
	const Core::Mat44f& localTx = transform.GetLocalTx();

	pWorld->m_pCameraSubsystem->PushCamera(m_pCamera);
	m_pCamera->SetLookAt(localTx.GetT() + m_cameraOffset, localTx.GetT(), Core::Vec4f(0, 1, 0, 0));
	m_pCamera->SetProjection(45 * Core::PI_OVER_180, 1920.f / 1080.f, 0.1f, 1000);
}

void PlayerComponent::Update(float dt)
{
	Systems::TransformComponent& transform = GetOwner()->GetTransform();
	const Core::Mat44f& localTx = transform.GetLocalTx();

	const Core::Vec4f forward = localTx.GetZ();
	const Core::Vec4f right = localTx.GetX();
	const Core::Vec4f oldPosition = localTx.GetT();

	Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();

	Core::Vec4f direction;
	if (inputMgr.GetState(Inputs::InputCommand::FORWARD))
		direction = direction + forward;
	else if(inputMgr.GetState(Inputs::InputCommand::BACKWARD))
		direction = direction - forward;

	if (inputMgr.GetState(Inputs::InputCommand::LEFT))
		direction = direction - right;
	else if (inputMgr.GetState(Inputs::InputCommand::RIGHT))
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
}

void PlayerComponent::OnDestroy(Systems::World* pWorld)
{
	pWorld->m_pCameraSubsystem->PopCamera();
}
