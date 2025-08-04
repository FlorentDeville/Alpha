/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"
#include "Systems/Reflection/FieldAttribute.h"

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

		template<typename T> void Init();

		bool IsInitialized() const;

		FieldDescriptor* AddField();

		void SetBaseType(const std::string& baseTypeName);
		void SetUpgradeType(Core::Sid upgradeType);

		const std::string& GetName() const;
		Core::Sid GetSid() const;
		const std::vector<FieldDescriptor*>& GetFields() const;
		const TypeDescriptor* GetBaseType() const;
		Core::Sid GetUpgradeType() const;

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

	private:
		std::string m_name;
		std::vector<FieldDescriptor*> m_fields;
		Core::Sid m_sid; //sid of m_name. It is deterministic, can be serialized and used to compare types.
		uint64_t m_size;
		const TypeDescriptor* m_pBaseType;
		Core::Sid m_upgradeType; // Sid of the type this class should be upgraded to
	};

	template<typename T> void TypeDescriptor::Init()
	{
		m_size = sizeof(T);
		Construct = []() -> void* { return new T(); };
		InPlaceConstruct = [](void* ptr) -> void* { return new(ptr) T(); };
		Destruct = [](void* pObject) { delete reinterpret_cast<T*>(pObject); };
	}
}