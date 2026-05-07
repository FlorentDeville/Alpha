/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/Boss/BossComponent.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Bullets/Waves/WaveTest.h"
#include "Alpha/StateMachine/StateMachine.h"
#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Components/Boss/States/BossStateWait.h"
#include "Alpha/Components/Boss/States/BossStateWaveTest.h"

#include "Core/Math/Constants.h"
#include "Core/Math/Vec4f.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"

#include "Systems/Game/GameMgr.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Objects/GameObject.h"

BossComponent::BossComponent()
	: GameComponent()
	, m_pWave(nullptr)
	, m_waveIndex(0)
	, m_pStateMachine(nullptr)
{ }

BossComponent::~BossComponent()
{
	delete m_pWave;
	delete m_pStateMachine;
}

void BossComponent::PostLoad()
{ }

void BossComponent::OnStartGame()
{
	Systems::GameObject* pObject = GetOwner();
	Systems::TransformComponent& transform = pObject->GetTransform();
	transform.ComputeWorldTx();

	m_pWave = new WaveTest(m_mesh.GetPtr(), m_material.GetPtr());

	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	m_waveIndex = pSubsystem->AddWave(m_pWave);
	pSubsystem->InitWave(m_waveIndex);
	pSubsystem->StartWave(m_waveIndex, transform.GetWorldTx().GetT());

	m_pStateMachine = new StateMachine();
	m_pStateMachine->Init(2);

	BossStateWait* pStateWait = new BossStateWait(m_pStateMachine);
	BossStateWaveTest* pStateWaveTest = new BossStateWaveTest(m_pStateMachine);

	m_pStateMachine->AddState(pStateWait, BossStateEnum::WAIT);
	m_pStateMachine->AddState(pStateWaveTest, BossStateEnum::WAVE_TEST);

	m_pStateMachine->Start(BossStateEnum::WAIT);
}

void BossComponent::Update(float dt)
{
	m_pStateMachine->Update();

	Move(dt);

	if (!m_pWave->IsAlive())
	{
		Systems::GameObject* pObject = GetOwner();
		Systems::TransformComponent& transform = pObject->GetTransform();

		BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
		pSubsystem->StartWave(m_waveIndex, transform.GetWorldTx().GetT());
	}
}

void BossComponent::OnDestroyGame()
{
	BulletSubsystem* pSubsystem = BulletSubsystem::GetSubsystem();
	pSubsystem->RemoveWave(m_waveIndex);

	delete m_pWave;
	m_pWave = nullptr;

	delete m_pStateMachine;
	m_pStateMachine = nullptr;
}

void BossComponent::Move(float dt)
{
	//dummy move to see it working
	Systems::GameObject* pObject = GetOwner();
	Systems::TransformComponent& transform = pObject->GetTransform();

	const Core::Vec4f speed(2, 0, 2, 0);

	Core::Sqt& loc = transform.GetLocalSqt();
	Core::Vec4f newPos = loc.GetTranslation() + speed * dt;

	loc.SetTranslation(newPos);
}
