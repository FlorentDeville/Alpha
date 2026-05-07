/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/BossComponent.h"

#include "Alpha/Bullets/BulletSubsystem.h"
#include "Alpha/Bullets/Waves/WaveTest.h"

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
{ }

BossComponent::~BossComponent()
{
	delete m_pWave;
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
}

void BossComponent::Update(float dt)
{ 
	//dummy move to see it working
	Systems::GameObject* pObject = GetOwner();
	Systems::TransformComponent& transform = pObject->GetTransform();

	const Core::Vec4f speed(2, 0, 2, 0);

	Core::Sqt& loc = transform.GetLocalSqt();
	Core::Vec4f newPos = loc.GetTranslation() + speed * dt;

	loc.SetTranslation(newPos);

	if (!m_pWave->IsAlive())
	{
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
}
