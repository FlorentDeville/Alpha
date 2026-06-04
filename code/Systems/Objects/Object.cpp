/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/Object.h"

namespace Systems
{
	Object::Object()
		: m_pTypeDescriptor(nullptr)
	{ }

	void Object::SetTypeDescriptor(const Core::TypeDescriptor* pTypeDescriptor)
	{
		m_pTypeDescriptor = pTypeDescriptor;
	}

	const Core::TypeDescriptor* Object::GetTypeDescriptor() const
	{
		return m_pTypeDescriptor;
	}

	void Object::PostLoad()
	{ }

	bool Object::IsA(Core::Sid type) const
	{
		return m_pTypeDescriptor->GetSid() == type || m_pTypeDescriptor->InheritsFrom(type);
	}

	Object* CreateObject(const Core::TypeDescriptor* pType)
	{
		Object* pObject = static_cast<Object*>(pType->Construct());
		pObject->SetTypeDescriptor(pType);

		return pObject;
	}
}
