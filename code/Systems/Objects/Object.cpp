/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/Object.h"

namespace Systems
{
	void Object::SetTypeDescriptor(const TypeDescriptor* pTypeDescriptor)
	{
		m_pTypeDescriptor = pTypeDescriptor;
	}

	const TypeDescriptor* Object::GetTypeDescriptor() const
	{
		return m_pTypeDescriptor;
	}
}
