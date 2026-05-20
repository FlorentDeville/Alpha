/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/BossGameObject.h"

#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Components/Boss/States/BossState_Phase1_Travel.h"
#include "Alpha/Components/Boss/States/BossState_Phase1_Attack.h"
#include "Alpha/Components/Boss/States/BossState_Phase2_Travel.h"
#include "Alpha/Components/Boss/States/BossState_Phase2_Attack1.h"
#include "Alpha/Components/Boss/States/BossState_Phase2_Attack2.h"
#include "Alpha/Objects/PlayerGameObject.h"
#include "Alpha/StateMachine/StateMachine.h"

#include "Systems/Game/GameMgr.h"
#include "Systems/Game/Subsystems/Message/GameMessage.h"
#include "Systems/Game/Subsystems/Navmesh/NavmeshSubsystem.h"

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
	m_pStateMachine->Init(BossStateEnum::COUNT);

	BossState_Phase1_Travel* pStatePhase1Travel = new BossState_Phase1_Travel(m_pStateMachine, this);
	BossState_Phase1_Attack* pStateWaveTest = new BossState_Phase1_Attack(m_pStateMachine);
	BossState_Phase2_Travel* pStatePhase2Travel = new BossState_Phase2_Travel(m_pStateMachine, this);
	BossState_Phase2_Attack1* pStatePhase2Attack1 = new BossState_Phase2_Attack1(m_pStateMachine);
	BossState_Phase2_Attack2* pStatePhase2Attack2 = new BossState_Phase2_Attack2(m_pStateMachine);

	pStateWaveTest->Init(m_mesh.GetPtr(), m_material.GetPtr(), m_counterBulletMaterial.GetPtr(), this, pPlayer);
	pStatePhase2Attack1->Init(m_mesh.GetPtr(), m_material.GetPtr(), m_counterBulletMaterial.GetPtr(), this, pPlayer);
	pStatePhase2Attack2->Init(m_mesh.GetPtr(), m_material.GetPtr(), m_counterBulletMaterial.GetPtr(), this, pPlayer);

	m_pStateMachine->AddState(pStatePhase1Travel, BossStateEnum::PHASE1_TRAVEL);
	m_pStateMachine->AddState(pStateWaveTest, BossStateEnum::PHASE1_ATTACK);
	m_pStateMachine->AddState(pStatePhase2Travel, BossStateEnum::PHASE2_TRAVEL);
	m_pStateMachine->AddState(pStatePhase2Attack1, BossStateEnum::PHASE2_ATTACK1);
	m_pStateMachine->AddState(pStatePhase2Attack2, BossStateEnum::PHASE2_ATTACK2);

	//m_pStateMachine->Start(BossStateEnum::PHASE1_TRAVEL);
	m_pStateMachine->Start(BossStateEnum::PHASE2_TRAVEL);

	Systems::CollisionSphereComponent* pCollision = m_collComp.FindComponent(this);
	pCollision->GetSphere().OnCollision([this](const Systems::ICollisionShape* pOther) { OnCollision(pOther); });

	Systems::NavmeshSubsystem* pNavmesh = Systems::NavmeshSubsystem::GetSubsystem();

	Core::Vec4f quad[4];
	quad[0] = Core::Vec4f(-48, 1.5, 46, 1);
	quad[1] = Core::Vec4f(47, 1.5, 46, 1);
	quad[2] = Core::Vec4f(47, 1.5, -44, 1);
	quad[3] = Core::Vec4f(-48, 1.5, -44, 1);

	pNavmesh->AddQuad(quad);
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
		//m_currentHP -= 500;
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
	if (m_currentHP < 0)
		m_currentHP = 0;

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
