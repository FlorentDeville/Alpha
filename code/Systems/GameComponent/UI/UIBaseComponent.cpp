/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/UI/UIBaseComponent.h"

namespace Systems
{
	UIBaseComponent::UIBaseComponent()
		: GameComponent()
		, m_debugName()
		, m_material()
		, m_position()
		, m_scale(1, 1)
	{ }

	UIBaseComponent::~UIBaseComponent()
	{ }

	const Core::Float2 UIBaseComponent::GetPosition() const
	{
		return m_position;
	}

	const Core::Float2 UIBaseComponent::GetScale() const
	{
		return m_scale;
	}

	void UIBaseComponent::SetPosition(const Core::Float2& pos)
	{
		m_position = pos;
	}

	void UIBaseComponent::SetScale(const Core::Float2& scale)
	{
		m_scale = scale;
	}
}
