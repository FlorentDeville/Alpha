/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Reflection/ReflectionMacro.h"
#include "Systems/Reflection/ReflectionMgr.h"

namespace Systems
{
	class TypeDescriptor;

	// Base class for any object editable and serializable.
	ENABLE_REFLECTION_WITH_NS(Systems, Object)
	class Object
	{
	public:
		Object() = default;
		virtual ~Object() = default;

		void SetTypeDescriptor(const TypeDescriptor* pTypeDescriptor);

	private:
		const TypeDescriptor* m_pTypeDescriptor;

		START_REFLECTION(Systems::Object)
		END_REFLECTION()
	};

	template<typename T, typename... Args> T* CreateObject(Args...)
	{
		T* pNewObject = new T(Args...);

		const TypeDescriptor* pType = TypeResolver<T>::GetConstType();
		pNewObject->SetTypeDescriptor(pType);

		return pNewObject;
	}
}
