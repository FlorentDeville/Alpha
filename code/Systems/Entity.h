/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/EntityId.h"

#include <bitset>

namespace Systems
{
	const int MAX_COMPONENTS = 32;
	
	typedef std::bitset<MAX_COMPONENTS> ComponentMask;

	class Entity
	{
	public:
		Entity(EntityId id);

		bool IsValid() const;
	
		void MakeInvalid();

		ComponentMask& GetMask();
		const ComponentMask& GetConstMask() const;

	private:
		EntityId m_id;
		ComponentMask m_mask;
	};
}
