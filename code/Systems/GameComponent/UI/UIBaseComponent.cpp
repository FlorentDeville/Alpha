/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/GameComponent/UI/UIBaseComponent.h"

namespace Systems
{
	UIBaseComponent::UIBaseComponent()
		: GameComponent()
		, m_debugName()
		, m_texture()
		, m_position(0, 0)
		, m_size(10, 10)
	{ }

	UIBaseComponent::~UIBaseComponent()
	{ }

	const Texture2DAsset* UIBaseComponent::GetTexture() const
	{
		return m_texture.GetPtr();
	}

	const Core::Float2 UIBaseComponent::GetPosition() const
	{
		return m_position;
	}

	const Core::Float2 UIBaseComponent::GetSize() const
	{
		return m_size;
	}

	void UIBaseComponent::SetPosition(const Core::Float2& pos)
	{
		m_position = pos;
	}

	void UIBaseComponent::SetSize(const Core::Float2& size)
	{
		m_size = size;
	}
}
