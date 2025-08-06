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
	public:
		// This constructor creates a non initialized TypeDescriptor.
		TypeDescriptor(const std::string& name);

		~TypeDescriptor();

		// Generic function to initialize the TypeDescriptor. I pass a dummy parameter otherwise I can't overload the function to specialize it.
		template<typename T> void Init(T* ptr = nullptr);

		// Initialize a Core::Array. I pass a dummy parameter otherwise I can't overload the function to specialize it.
		template<typename T> void Init(Core::Array<T>* ptr = nullptr);

		bool IsInitialized() const;

		FieldDescriptor* AddField();

		void SetBaseType(const std::string& baseTypeName);
		void SetUpgradeType(Core::Sid upgradeType);

		const std::string& GetName() const;
		Core::Sid GetSid() const;
		const std::vector<FieldDescriptor*>& GetFields() const;
		const TypeDescriptor* GetBaseType() const;
		const TypeDescriptor* GetElementType() const;
		Core::Sid GetUpgradeType() const;

		bool IsContainer() const;
		bool IsElementPointer() const;

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
		Core::Sid m_upgradeType;			// Sid of the type this class should be upgraded to
		bool m_isContainer;					// true if this is an array, m_pElementType is the type of the elements in the array.
		bool m_isElementPointer;			// this is an array of pointer to m_pElementType.
		TypeDescriptor* m_pElementType;		// this is the type of the elements when the field is a container. it must be iteratable with begin/end.
	};

	template<typename T> void TypeDescriptor::Init(T* /*ptr*/)
	{
		m_size = sizeof(T);
		Construct = []() -> void* { return new T(); };
		InPlaceConstruct = [](void* ptr) -> void* { return new(ptr) T(); };
		Destruct = [](void* pObject) { delete reinterpret_cast<T*>(pObject); };
		Copy = [](const void* pSrc, void* pDst) { *reinterpret_cast<T*>(pDst) = *reinterpret_cast<const T*>(pSrc); };
	}

	template<typename T> void TypeDescriptor::Init(Core::Array<T>* /*ptr*/)
	{
		m_size = sizeof(Core::Array<T>);
		Construct = []() -> void* { return new Core::Array<T>(); };
		InPlaceConstruct = [](void* ptr) -> void* { return new(ptr) Core::Array<T>(); };
		Destruct = [](void* pObject) { delete reinterpret_cast<Core::Array<T>*>(pObject); };
		Copy = [](const void* pSrc, void* pDst) { *reinterpret_cast<Core::Array<T>*>(pDst) = *reinterpret_cast<const Core::Array<T>*>(pSrc); };

		m_isContainer = true;

		using NonPointerElementType = typename RemovePointer<T>::type;
		m_pElementType = TypeResolver<NonPointerElementType>::GetType();

		m_isElementPointer = IsPointer<T>::value;
	}
}