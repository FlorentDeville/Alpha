/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/PlayerComponent.h"

#include "Core/Math/Constants.h"

#include "Inputs/InputMgr.h"

#include "OsWin/VirtualKeyCode.h"

#include "Rendering/Camera.h"

#include "Systems/Game/GameMgr.h"
#include "Systems/Objects/GameObject.h"

PlayerComponent::PlayerComponent()
	: GameComponent()
	, m_speed(1)
	, m_pCamera(nullptr)
	, m_isCameraRegistered(false)
{
	m_pCamera = new Rendering::Camera();
}

PlayerComponent::~PlayerComponent()
{
	delete m_pCamera;
}

void PlayerComponent::PostLoad()
{
}

void PlayerComponent::Update()
{
	Systems::TransformComponent& transform = GetOwner()->GetTransform();
	const Core::Mat44f& localTx = transform.GetLocalTx();

	const Core::Vec4f cameraOffset(0, 10, -10, 0);

	if (!m_isCameraRegistered)
	{
		Systems::GameMgr::Get().PushCamera(m_pCamera);
		m_pCamera->SetLookAt(localTx.GetT() + cameraOffset, localTx.GetT(), Core::Vec4f(0, 1, 0, 0));
		m_pCamera->SetProjection(45 * Core::PI_OVER_180, 1920.f / 1080.f, 0.1f, 1000);
	}

	const Core::Vec4f forward = localTx.GetZ();
	const Core::Vec4f right = localTx.GetX();
	const Core::Vec4f oldPosition = localTx.GetT();

	Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();

	Core::Vec4f direction;
	if (inputMgr.IsKeyPressed(Os::VKeyCodes::Vk_W))
		direction = direction + forward;
	else if(inputMgr.IsKeyPressed(Os::VKeyCodes::Vk_S))
		direction = direction - forward;

	if (inputMgr.IsKeyPressed(Os::VKeyCodes::Vk_A))
		direction = direction - right;
	else if (inputMgr.IsKeyPressed(Os::VKeyCodes::Vk_D))
		direction = direction + right;

	if (direction.Length() != 0)
	{
		direction.Normalize();
		Core::Vec4f newPosition = oldPosition + direction * m_speed;

		Core::Mat44f newLocalTx = localTx;
		newLocalTx.SetRow(3, newPosition);

		transform.SetLocalTx(newLocalTx);

		m_pCamera->SetLookAt(localTx.GetT() + cameraOffset, localTx.GetT(), Core::Vec4f(0, 1, 0, 0));
	}
}
