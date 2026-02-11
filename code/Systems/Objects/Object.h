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
		Object();
		virtual ~Object() = default;

		void SetTypeDescriptor(const TypeDescriptor* pTypeDescriptor);

		const TypeDescriptor* GetTypeDescriptor() const;

		virtual void PostLoad();

		template<class T> bool IsA() const;

		template<class T> T* Cast();
		template<class T> const T* Cast() const;

	private:
		const TypeDescriptor* m_pTypeDescriptor;

		START_REFLECTION(Systems::Object)
		END_REFLECTION()
	};

	template<class T> bool Object::IsA() const
	{
		return m_pTypeDescriptor->GetSid() == TypeResolver<T>::GetTypenameSid();
	}

	template<class T> T* Object::Cast()
	{
		if (!IsA<T>())
			return nullptr;

		return static_cast<T*>(this);
	}

	template<class T> const T* Object::Cast() const
	{
		if (!IsA<T>())
			return nullptr;

		return static_cast<const T*>(this);
	}

	Object* CreateObject(const TypeDescriptor* pType);

	template<typename T, typename... Args> T* CreateObject(Args... args)
	{
		T* pNewObject = new T(std::forward<Args>(args)...);

		const TypeDescriptor* pType = TypeResolver<T>::GetConstType();
		pNewObject->SetTypeDescriptor(pType);

		return pNewObject;
	}

	template<typename T, typename... Args> T* CreateObjectInPlace(T* pPtr, Args... args)
	{
		new(pPtr) T(std::forward<Args>(args)...);

		const TypeDescriptor* pType = TypeResolver<T>::GetConstType();
		pPtr->SetTypeDescriptor(pType);

		return pPtr;
	}

	template<typename T> void DeleteObject(T* pObject)
	{
		delete pObject;
	}
}
