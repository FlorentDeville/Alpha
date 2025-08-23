/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Sid/Sid.h"

#include "Systems/Reflection/FieldAttribute.h"
#include "Systems/Reflection/ReflectionUtils.h"
#include "Systems/Reflection/TypeResolver.h"

#include <string>
#include <vector>

namespace Systems
{
	class FieldDescriptor;

	class TypeDescriptor
	{
		template<typename T> friend class TypeInitializer;

	public:
		// This constructor creates a non initialized TypeDescriptor.
		TypeDescriptor(const std::string& name);

		~TypeDescriptor();

		bool IsInitialized() const;

		FieldDescriptor* AddField();

		void SetBaseType(const std::string& baseTypeName);
		void SetUpgradeType(Core::Sid upgradeType);

		const std::string& GetName() const;
		Core::Sid GetSid() const;
		const std::vector<FieldDescriptor*>& GetFields() const;
		const TypeDescriptor* GetBaseType() const;
		const TypeDescriptor* GetTemplateParamType() const;
		Core::Sid GetUpgradeType() const;
		uint64_t GetSize() const;

		bool IsContainer() const;
		bool IsTemplateParamTypePointer() const;

		// True if this type inherits from baseClassSid
		bool InheritsFrom(Core::Sid baseClassSid) const;

		// True if this class inehrits from Object.
		bool IsObject() const;

		// True if this class inherits from GameObject.
		bool IsGameObject() const;

		// True if the class ihnerits from GameComponent.
		bool IsGameComponent() const;

		// True if this is a class (it has fields). False if it's a pod (there is no field).
		bool IsClass() const;

		// Create an object of the type. (Allocate memory and call the no parameter constructor)
		void* (*Construct)();

		//Call in place constructor to construct a type
		void* (*InPlaceConstruct)(void*);

		// Delete an object of the type.
		void (*Destruct)(void*);

		//Function to call to upgrade this type to the type m_upgradeType
		void (*Upgrade)(const void* src, void* dst);

		// Copy using copy operator the object src to dst.
		void (*Copy)(const void* pSrc, void* pDst);

	private:
		std::string m_name;
		std::vector<FieldDescriptor*> m_fields;
		Core::Sid m_sid; //sid of m_name. It is deterministic, can be serialized and used to compare types.
		uint64_t m_size;
		const TypeDescriptor* m_pBaseType;
		Core::Sid m_upgradeType;				// Sid of the type this class should be upgraded to
		
		bool m_isContainer : 1;					// True if this is an array, m_pTemplateType is the type of the elements in the array. Can be iterated with begin/end.
		bool m_isTemplate : 1;					// True if this is a templated type. Arrays are templated types.
		bool m_isTemplateParamTypePointer : 1;	// True if the current type uses a pointer to a templated type. For example Core::Array<T*>.

		// This is the type of the template parameter. For example it is T in Core::Array<T> or A in HardAssetRed<A>.
		// TypeDescriptor always represents the type itself, never a pointer to the type. So if the template type is T*, thwn
		// m_isTemplateTypePointer is true.
		TypeDescriptor* m_pTemplateParamType;
	};

	template<typename T> class TypeInitializer
	{
	public:
		static void Run(TypeDescriptor* pType)
		{
			pType->m_size = sizeof(T);
			pType->Construct = []() -> void* { return new T(); };
			pType->InPlaceConstruct = [](void* ptr) -> void* { return new(ptr) T(); };
			pType->Destruct = [](void* pObject) { delete reinterpret_cast<T*>(pObject); };
			pType->Copy = [](const void* pSrc, void* pDst) { *reinterpret_cast<T*>(pDst) = *reinterpret_cast<const T*>(pSrc); };
		}
	};

	template<typename T> class TypeInitializer<Core::Array<T>>
	{
	public:
		static void Run(TypeDescriptor* pType)
		{
			pType->m_size = sizeof(Core::Array<T>);
			pType->Construct = []() -> void* { return new Core::Array<T>(); };
			pType->InPlaceConstruct = [](void* ptr) -> void* { return new(ptr) Core::Array<T>(); };
			pType->Destruct = [](void* pObject) { delete reinterpret_cast<Core::Array<T>*>(pObject); };
			pType->Copy = [](const void* pSrc, void* pDst) { *reinterpret_cast<Core::Array<T>*>(pDst) = *reinterpret_cast<const Core::Array<T>*>(pSrc); };

			pType->m_isContainer = true;
			pType->m_isTemplate = true;

			using NonPointerElementType = typename RemovePointer<T>::type;
			pType->m_pTemplateParamType = TypeResolver<NonPointerElementType>::GetType();

			pType->m_isTemplateParamTypePointer = IsPointer<T>::value;
		}
	};
}