/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Objects/BossGameObject.h"

#include "Alpha/Components/Boss/States/BossStateEnum.h"
#include "Alpha/Components/Boss/States/BossState_Phase1_Travel.h"
#include "Alpha/Components/Boss/States/BossState_Phase1_Attack.h"
#include "Alpha/Components/Boss/States/BossState_Phase1_To_Phase2.h"
#include "Alpha/Components/Boss/States/BossState_Phase2_Travel.h"
#include "Alpha/Components/Boss/States/BossState_Phase2_Attack1.h"
#include "Alpha/Components/Boss/States/BossState_Phase2_Attack2.h"
#include "Alpha/Components/Boss/States/BossState_Phase2_To_Phase3.h"
#include "Alpha/Components/Boss/States/BossState_Phase3_Travel.h"
#include "Alpha/Components/Boss/States/BossState_Phase3_Attack1.h"
#include "Alpha/Components/Boss/States/BossState_Phase3_Attack2.h"
#include "Alpha/Components/Boss/States/BossState_Start.h"
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

	BossState_Start* pStateStart = new BossState_Start(m_pStateMachine, this);
	BossState_Phase1_Travel* pStatePhase1Travel = new BossState_Phase1_Travel(m_pStateMachine, this);
	m_pStatePhase1Attack = new BossState_Phase1_Attack(m_pStateMachine);
	BossState_Phase1_To_Phase2* pStatePhase1ToPhase2 = new BossState_Phase1_To_Phase2(m_pStateMachine, this);
	BossState_Phase2_Travel* pStatePhase2Travel = new BossState_Phase2_Travel(m_pStateMachine, this);
	m_pStatePhase2Attack1 = new BossState_Phase2_Attack1(m_pStateMachine);
	m_pStatePhase2Attack2 = new BossState_Phase2_Attack2(m_pStateMachine);
	BossState_Phase2_To_Phase3* pStatePhase2ToPhase3 = new BossState_Phase2_To_Phase3(m_pStateMachine, this);
	BossState_Phase3_Travel* pStatePhase3Travel = new BossState_Phase3_Travel(m_pStateMachine, this);
	m_pStatePhase3Attack1 = new BossState_Phase3_Attack1(m_pStateMachine);
	m_pStatePhase3Attack2 = new BossState_Phase3_Attack2(m_pStateMachine);

	m_pStatePhase1Attack->Init(m_mesh.GetPtr(), m_material.GetPtr(), m_counterBulletMaterial.GetPtr(), this, pPlayer);
	m_pStatePhase2Attack1->Init(m_mesh.GetPtr(), m_material.GetPtr(), m_counterBulletMaterial.GetPtr(), this, pPlayer);
	m_pStatePhase2Attack2->Init(m_mesh.GetPtr(), m_material.GetPtr(), m_counterBulletMaterial.GetPtr(), this, pPlayer);
	m_pStatePhase3Attack1->Init(m_mesh.GetPtr(), m_material.GetPtr(), m_counterBulletMaterial.GetPtr(), this, pPlayer);
	m_pStatePhase3Attack2->Init(m_mesh.GetPtr(), m_material.GetPtr(), m_counterBulletMaterial.GetPtr(), this, pPlayer);

	m_pStateMachine->AddState(pStateStart, BossStateEnum::START);
	m_pStateMachine->AddState(pStatePhase1Travel, BossStateEnum::PHASE1_TRAVEL);
	m_pStateMachine->AddState(m_pStatePhase1Attack, BossStateEnum::PHASE1_ATTACK);
	m_pStateMachine->AddState(pStatePhase1ToPhase2, BossStateEnum::PHASE1_TO_PHASE2);
	m_pStateMachine->AddState(pStatePhase2Travel, BossStateEnum::PHASE2_TRAVEL);
	m_pStateMachine->AddState(m_pStatePhase2Attack1, BossStateEnum::PHASE2_ATTACK1);
	m_pStateMachine->AddState(m_pStatePhase2Attack2, BossStateEnum::PHASE2_ATTACK2);
	m_pStateMachine->AddState(pStatePhase2ToPhase3, BossStateEnum::PHASE2_TO_PHASE3);
	m_pStateMachine->AddState(pStatePhase3Travel, BossStateEnum::PHASE3_TRAVEL);
	m_pStateMachine->AddState(m_pStatePhase3Attack1, BossStateEnum::PHASE3_ATTACK1);
	m_pStateMachine->AddState(m_pStatePhase3Attack2, BossStateEnum::PHASE3_ATTACK2);

	m_pStateMachine->Start(BossStateEnum::START);
	//m_pStateMachine->Start(BossStateEnum::PHASE1_TRAVEL);
	//m_pStateMachine->Start(BossStateEnum::PHASE3_TRAVEL);

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
		m_currentHP -= 100;
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

uint32_t BossGameObject::GetMaxHP() const
{
	return m_maxHP;
}

int32_t BossGameObject::GetCurrentHP() const
{
	return m_currentHP;
}

void BossGameObject::SetCurrentHP(int32_t hp)
{
	m_currentHP = hp;
	UpdateHPBar();
}

void BossGameObject::EnterPhase1()
{
	m_pStatePhase1Attack->InitWaves();
	SetCurrentHP(GetMaxHP());
}

void BossGameObject::ExitPhase1()
{
	m_pStatePhase1Attack->DestroyWaves();
}

void BossGameObject::EnterPhase2()
{
	m_pStatePhase2Attack1->InitWaves();
	m_pStatePhase2Attack2->InitWaves();
}

void BossGameObject::ExitPhase2()
{
	m_pStatePhase2Attack1->DestroyWaves();
	m_pStatePhase2Attack2->DestroyWaves();
}

void BossGameObject::EnterPhase3()
{

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
