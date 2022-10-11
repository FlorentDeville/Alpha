/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Entities/PlayerEntity.h"

#include "Inputs/InputMgr.h"

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

	const Inputs::InputMgr& inputMgr = Inputs::InputMgr::Get();

	float speed = 1.f;
	if (inputMgr.GetState(Inputs::InputCommand::MoveForward))
		m_position = DirectX::XMVectorAdd(m_position, DirectX::XMVectorSet(0, 0, speed, 0));

	if (inputMgr.GetState(Inputs::InputCommand::MoveBackward))
		m_position = DirectX::XMVectorAdd(m_position, DirectX::XMVectorSet(0, 0, -speed, 0));

	if (inputMgr.GetState(Inputs::InputCommand::MoveLeft))
		m_position = DirectX::XMVectorAdd(m_position, DirectX::XMVectorSet(-speed, 0, 0, 0));

	if (inputMgr.GetState(Inputs::InputCommand::MoveRight))
		m_position = DirectX::XMVectorAdd(m_position, DirectX::XMVectorSet(speed, 0, 0, 0));

	SetTransform(DirectX::XMMatrixTranslationFromVector(m_position));
}
