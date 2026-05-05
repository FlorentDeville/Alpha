/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Alpha/Bullets/BulletSubsystem.h"

BulletSubsystem::BulletSubsystem()
	: Systems::ISubsystem()
{ }

BulletSubsystem::~BulletSubsystem()
{ }

void BulletSubsystem::Update(const Systems::GameContext& /*context*/)
{ }

void BulletSubsystem::BuildRenderable(Systems::RenderableScene& /*scene*/)
{ }
