/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Reflection/FieldDescriptor.h"
#include "Systems/Reflection/ReflectionMgr.h"

// Define macros to enable reflection of a class
// To have reflection working :
//  - Create an instance of TypeDescriptor. This class describes a type. 
//  - Register the TypeDescriptor to the ReflectionMgr.
//  - Create a class TypeResolver<TYPE>. It contains functions to query the ReflectionMgr to find the TypeDescriptor.
//
// Here are the macros used to (almost) automatically generates all of this :
// ENABLE_REFLECTION : Creates class TypeResolver<TYPE>.
// 
// START_REFLECTION : Creates function static void RegisterReflection(). This function creates a TypeDescriptor.
// ADD_FIELD : Add a field to the TypeDescriptor.
// ADD_BASETYPE : Add basetype to the TypeDescriptor.
// END_REFLECTION : Close RegisterReflection.
// 
// Then somewhere you need to call MyClass::RegisterReflection();
// 
// If the type is a pod, you don't need to do all of this and can do this :
//  - DEFINE_TYPE_RESOLVER : to create the TypeResolver<Type>
//  - REGISTER_TYPE : use this macro in a cpp to register the type to ReflectionMgr
// 
// Here is an example:
// 
// ENABLE_REFLECTION(MyClass)
// class MyClass
// {
//	public:
//		int32_t m_i;
//		float m_f;
// private:
//		bool m_b;
// 
//		START_REFLECTION(MyClass)
//			// ADD_BASETYPE(MyBaseClass) // optional when there is no base class
//			ADD_FIELD(m_i)
//			ADD_FIELD(m_f)
//			ADD_FIELD(m_b)
//		END_REFLECTION()
// }
// 

#define OPEN_SYSTEMS_NAMESPACE namespace Systems {
#define CLOSE_SYSTEMS_NAMESPACE }

// Macro to specialize the template class TypeResolver
#define DEFINE_TYPE_RESOLVER(TYPE) \
	template<> class TypeResolver<TYPE> \
	{ \
	public: \
		static const std::string& GetTypename() { static const std::string typeName = #TYPE; return typeName; } \
		static const TypeDescriptor* GetConstType() { return Systems::ReflectionMgr::Get().GetOrAddType(GetTypename()); } \
		static TypeDescriptor* GetType() { return Systems::ReflectionMgr::Get().GetOrAddType(GetTypename()); } \
	};

// Macro to specialize the template class TypeResolver inside the Systems namespace
#define DEFINE_SYSTEMS_TYPE_RESOLVER(TYPE) \
	OPEN_SYSTEMS_NAMESPACE \
		DEFINE_TYPE_RESOLVER(TYPE) \
	CLOSE_SYSTEMS_NAMESPACE

// Macro to register a type
#define REGISTER_TYPE(TYPE) \
	{ \
		Systems::TypeDescriptor* pType = Systems::ReflectionMgr::Get().RegisterType<TYPE>(#TYPE); \
		pType->Init<TYPE>(); \
	}

#define REGISTER_FIELD(DESCRIPTOR, TYPE, FIELD_TYPE, FIELD_NAME, ATTRIBUTE) \
	{ \
		Systems::FieldDescriptor* pNewField = DESCRIPTOR->AddField(); \
		Systems::FieldInitializer<FIELD_TYPE>::Run(pNewField, #FIELD_NAME, offsetof(TYPE, FIELD_NAME), Systems::FieldAttribute::ATTRIBUTE); \
	}

// Macro to put first before the class
#define ENABLE_REFLECTION_WITH_NS(NAMESPACE, TYPE) \
	class TYPE; \
	DEFINE_TYPE_RESOLVER(NAMESPACE::TYPE)

// Macro to put first before the class
#define ENABLE_REFLECTION(TYPE) \
	class TYPE; \
	DEFINE_TYPE_RESOLVER(TYPE)

// Macro to put first before the class
#define ENABLE_SYSTEMS_REFLECTION(TYPE) \
	class TYPE; \
	DEFINE_SYSTEMS_TYPE_RESOLVER(TYPE)

// Macro to start the description of the reflection
#define START_REFLECTION(TYPE) \
public: \
	static void RegisterReflection() {\
		using ClassType = TYPE; \
		Systems::TypeDescriptor* pType = Systems::ReflectionMgr::Get().RegisterType<TYPE>(#TYPE); \
		pType->Init<ClassType>();
	
// Macro to end the description of the reflection
#define END_REFLECTION() }

// Macro to add a field to the reflection
#define ADD_FIELD(FIELD_NAME) REGISTER_FIELD(pType, ClassType, decltype(FIELD_NAME), FIELD_NAME, None)
#define ADD_FIELD_ATTR(FIELD_NAME, ATTRIBUTE) REGISTER_FIELD(pType, ClassType, decltype(FIELD_NAME), FIELD_NAME, ATTRIBUTE)

// Macro to add a base type to the current type
#define ADD_BASETYPE(BASETYPE) pType->SetBaseType(#BASETYPE);

// Macro the set the upgrade type
#define ADD_UPGRADE_TYPE(TYPE, FUNC) \
	pType->SetUpgradeType(MAKESID(#TYPE)); \
	pType->Upgrade = [](const void* pSrc, void* pDst) { FUNC(static_cast<const ClassType*>(pSrc), static_cast<TYPE*>(pDst)); };
	