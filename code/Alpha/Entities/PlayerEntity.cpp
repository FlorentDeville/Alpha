/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Entities/PlayerEntity.h"

#include "GameInputs/Inputs/InputMgr.h"

PlayerEntity::PlayerEntity()
	: LocatorEntity()
	, m_position()
	, m_euler()
	, m_scale()
{
	m_position = DirectX::XMVectorSet(0, 1, 0, 1);
}

PlayerEntity::~PlayerEntity()
{}

void PlayerEntity::Update()
{
	LocatorEntity::Update();

	const GameInputs::InputMgr& inputMgr = GameInputs::InputMgr::Get();

	float speed = 1.f;
	if (inputMgr.GetState(GameInputs::InputCommand::MoveForward))
		m_position = DirectX::XMVectorAdd(m_position, DirectX::XMVectorSet(0, 0, speed, 0));

	if (inputMgr.GetState(GameInputs::InputCommand::MoveBackward))
		m_position = DirectX::XMVectorAdd(m_position, DirectX::XMVectorSet(0, 0, -speed, 0));

	if (inputMgr.GetState(GameInputs::InputCommand::MoveLeft))
		m_position = DirectX::XMVectorAdd(m_position, DirectX::XMVectorSet(-speed, 0, 0, 0));

	if (inputMgr.GetState(GameInputs::InputCommand::MoveRight))
		m_position = DirectX::XMVectorAdd(m_position, DirectX::XMVectorSet(speed, 0, 0, 0));

	SetTransform(DirectX::XMMatrixTranslationFromVector(m_position));
}
