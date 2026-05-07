/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

namespace Systems
{
	class GameContext;
	class RenderableScene;

	class ISubsystem
	{
	public:
		virtual ~ISubsystem() {}

		virtual void Update(const GameContext& /*context*/) {};
		virtual void BuildRenderable(RenderableScene& /*scene*/) {};
	};
}
