/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Entity.h"

namespace Systems
{
	Entity::Entity(EntityId id)
		: m_id(id)
		, m_mask()
	{}

	bool Entity::IsValid() const
	{
		return m_id != INVALID_ENTITY_ID;
	}

	void Entity::MakeInvalid()
	{
		m_id = INVALID_ENTITY_ID;
		m_mask.reset();
	}

	ComponentMask& Entity::GetMask()
	{
		return m_mask;
	}

	const ComponentMask& Entity::GetConstMask() const
	{
		return m_mask;
	}
}
