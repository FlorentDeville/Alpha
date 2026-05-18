/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/BossGameObject.h"

#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Components/Boss/States/BossStateWait.h"
#include "Alpha/Components/Boss/States/BossStateWaveTest.h"
#include "Alpha/Objects/PlayerGameObject.h"
#include "Alpha/StateMachine/StateMachine.h"

#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Message/GameMessage.h"

BossGameObject::BossGameObject()
	: BaseClass()
	, m_currentHP()
	, m_maxHP()
	, m_pStateMachine(nullptr)
{ }

BossGameObject::~BossGameObject()
{
	delete m_pStateMachine;
}

void BossGameObject::OnStartGame()
{
	BaseClass::OnStartGame();

	m_currentHP = m_maxHP;

	const PlayerGameObject* pPlayer = Systems::GameMgr().Get().FindGameObject<PlayerGameObject>();

	m_pStateMachine = new StateMachine();
	m_pStateMachine->Init(2);

	BossStateWait* pStateWait = new BossStateWait(m_pStateMachine);
	BossStateWaveTest* pStateWaveTest = new BossStateWaveTest(m_pStateMachine);
	pStateWaveTest->Init(m_mesh.GetPtr(), m_material.GetPtr(), m_counterBulletMaterial.GetPtr(), this, pPlayer);

	m_pStateMachine->AddState(pStateWait, BossStateEnum::WAIT);
	m_pStateMachine->AddState(pStateWaveTest, BossStateEnum::WAVE_TEST);

	m_pStateMachine->Start(BossStateEnum::WAIT);

	Systems::CollisionSphereComponent* pCollision = m_collComp.FindComponent(this);
	pCollision->GetSphere().OnCollision([this](const Systems::ICollisionShape* pOther) { OnCollision(pOther); });
}

void BossGameObject::Update(float dt)
{
	BaseClass::Update(dt);

	m_pStateMachine->Update();
}

void BossGameObject::HandleMessage(const Systems::GameMessage& msg)
{
	switch (msg.m_id)
	{
	case SID("bullet_counter_collision"):
	{
		m_currentHP -= 50;
		UpdateHPBar();
	}
	break;

	default:
		break;
	}
}

void BossGameObject::OnDestroyGame()
{
	BaseClass::OnDestroyGame();

	delete m_pStateMachine;
	m_pStateMachine = nullptr;
}

void BossGameObject::OnCollision(const Systems::ICollisionShape* pOther)
{
	Systems::GameObject* pOwner = pOther->GetOwner();
	if (!pOwner)
		return;

	if (!pOwner->IsA<PlayerGameObject>())
		return;

	//I collided with the player so reduce hp
	uint32_t damage = 10;
	m_currentHP -= damage;

	UpdateHPBar();
}

void BossGameObject::UpdateHPBar()
{
	Systems::UIBaseComponent* pTotalHp = m_totalHealthComp.FindComponent(this);
	Systems::UIBaseComponent* pHp = m_currentHealthComp.FindComponent(this);

	Core::Float2 size = pHp->GetSize();
	size.x = pTotalHp->GetSize().x / m_maxHP * m_currentHP;
	pHp->SetSize(size);

	Core::Float2 position = pHp->GetPosition();
	position.x = pTotalHp->GetPosition().x - ((pTotalHp->GetSize().x - size.x) * 0.5f);
	pHp->SetPosition(position);
}
