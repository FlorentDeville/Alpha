/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/UI/UITextureComponent.h"

namespace Systems
{
	UITextureComponent::UITextureComponent()
		: GameComponent()
		, m_debugName()
		, m_texture()
		, m_position()
		, m_scale(1, 1)
	{ }

	UITextureComponent::~UITextureComponent()
	{ }
}
