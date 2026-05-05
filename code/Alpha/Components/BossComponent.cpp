/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Components/BossComponent.h"

#include "Core/Math/Constants.h"
#include "Core/Math/Vec4f.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"

#include "Systems/Game/GameMgr.h"
#include "Systems/Game/GameContext.h"
#include "Systems/Objects/GameObject.h"

BossComponent::BossComponent()
	: GameComponent()
{ }

BossComponent::~BossComponent()
{ }

void BossComponent::PostLoad()
{ }

void BossComponent::OnStart(Systems::GameContext* /*pWorld*/)
{ }

void BossComponent::Update(float dt)
{ 
	//dummy move to see it working
	Systems::GameObject* pObject = GetOwner();
	Systems::TransformComponent& transform = pObject->GetTransform();

	const Core::Vec4f speed(2, 0, 2, 0);

	Core::Sqt& loc = transform.GetLocalSqt();
	Core::Vec4f newPos = loc.GetTranslation() + speed * dt;

	loc.SetTranslation(newPos);
}

void BossComponent::OnDestroy(Systems::GameContext* /*pWorld*/)
{ }
