/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/Object.h"

namespace Systems
{
	Object::Object()
		: m_pTypeDescriptor(nullptr)
	{ }

	void Object::SetTypeDescriptor(const TypeDescriptor* pTypeDescriptor)
	{
		m_pTypeDescriptor = pTypeDescriptor;
	}

	const TypeDescriptor* Object::GetTypeDescriptor() const
	{
		return m_pTypeDescriptor;
	}

	void Object::PostLoad()
	{ }

	Object* CreateObject(const TypeDescriptor* pType)
	{
		Object* pObject = static_cast<Object*>(pType->Construct());
		pObject->SetTypeDescriptor(pType);

		return pObject;
	}
}
