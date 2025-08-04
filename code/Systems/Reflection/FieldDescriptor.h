/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Systems/Reflection/FieldAttribute.h"
#include "Systems/Reflection/ReflectionMgr.h"
#include "Systems/Reflection/ReflectionUtils.h"
#include "Systems/Reflection/TypeDescriptor.h"
#include "Systems/Reflection/TypeResolver.h"

#include <string>

namespace Systems
{
	class TypeDescriptor;

	class FieldDescriptor
	{
		template<typename T> friend class FieldInitializer;

	public:
		FieldDescriptor();
		~FieldDescriptor();

		const std::string& GetName() const;
		const TypeDescriptor* GetType() const;
		const TypeDescriptor* GetElementType() const;
		bool IsPointer() const;
		bool IsContainer() const;
		bool IsElementPointer() const;

		//Return a pointer to the variable in pObj
		const void* GetDataPtr(const void* pObj) const;
		void* GetDataPtr(void* pObj) const;

		template<typename T> const T* GetDataPtr(const void* pObj) const;
		template<typename T> T* GetDataPtr(void* pObj) const;

		//Check if the attribute Hidden is present.
		bool IsHidden() const;

		bool IsReadOnly() const;

	private:
		std::string m_name;
		uint64_t m_offset;
		TypeDescriptor* m_pType;
		TypeDescriptor* m_pElementType; // this is the type of the elements when the field is a container. it must be iteratable with begin/end.
		bool m_isPointer : 1;
		bool m_isContainer : 1;			// array, map, list of any kind
		bool m_isElementPointer : 1;	// the elements are pointers to m_pElementType.

		FieldAttribute m_attribute;
	};

	template<typename T> const T* FieldDescriptor::GetDataPtr(const void* pObj) const
	{
		return reinterpret_cast<const T*>(GetDataPtr(pObj));
	}

	template<typename T> T* FieldDescriptor::GetDataPtr(void* pObj) const
	{
		return reinterpret_cast<T*>(GetDataPtr(pObj));
	}

	template<typename FIELD_TYPE> class FieldInitializer
	{
	public:
		static void Run(FieldDescriptor* pField, const std::string& name, size_t offset, FieldAttribute attribute)
		{
			typedef RemovePointer<FIELD_TYPE>::type NonPointerType;
			TypeDescriptor* pType = TypeResolver<NonPointerType>::GetType();

			bool isPointer = IsPointer<FIELD_TYPE>::value;

			pField->m_name = name;
			pField->m_offset = offset;
			pField->m_pType = pType;
			pField->m_pElementType = nullptr;
			pField->m_isPointer = isPointer;
			pField->m_isContainer = false;
			pField->m_isElementPointer = false;
			pField->m_attribute = attribute;
		}
	};

	template<typename T> class FieldInitializer<Core::Array<T>>
	{
	public:
		static void Run(FieldDescriptor* pField, const std::string& name, size_t offset, FieldAttribute attribute)
		{
			typedef RemovePointer<T>::type NonPointerElementType;
			TypeDescriptor* pElementType = TypeResolver<NonPointerElementType>::GetType();

			bool isElementPointer = IsPointer<T>::value;

			std::string arrayTypename = "Core::Array<" + pElementType->GetName();
			if (isElementPointer)
				arrayTypename += "*";

			arrayTypename += ">";

			TypeDescriptor* pArrayType = Systems::ReflectionMgr::Get().GetOrAddType(arrayTypename);
			if (!pArrayType->IsInitialized())
			{
				pArrayType->Init<Core::Array<T>>();
			}

			pField->m_name = name;
			pField->m_offset = offset;
			pField->m_pType = pArrayType;
			pField->m_pElementType = pElementType;
			pField->m_isPointer = false;
			pField->m_isContainer = true;
			pField->m_isElementPointer = isElementPointer;
			pField->m_attribute = attribute;
		}
	};
}
