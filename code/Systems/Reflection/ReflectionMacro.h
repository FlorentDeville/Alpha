/********************************************************************************/
/* Copyright (C) 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/FieldAttribute.h"
#include "Core/Reflection/FieldDescriptor.h"
#include "Core/Reflection/ReflectionMgr.h"
#include "Core/Reflection/TypeResolver.h"
#include "Core/Sid/Sid.h"

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

// Macro to specialize the template class TypeResolver
#define DEFINE_TYPE_RESOLVER(TYPE) \
	namespace Core { \
	template<> class TypeResolver<TYPE> \
	{ \
	public: \
		static const std::string& GetTypename() { static const std::string typeName = #TYPE; return typeName; } \
		static Core::Sid GetTypenameSid() { return CONSTSID(#TYPE); } \
		static const Core::TypeDescriptor* GetConstType() { return Core::ReflectionMgr::Get().GetOrAddType(GetTypename()); } \
		static Core::TypeDescriptor* GetType() { return Core::ReflectionMgr::Get().GetOrAddType(GetTypename()); } \
	}; \
	} // namespace Core

// Macro to register a type
#define REGISTER_TYPE(TYPE) \
	{ \
		Core::TypeDescriptor* pType = Core::ReflectionMgr::Get().RegisterType<TYPE>(#TYPE); \
		Core::TypeInitializer<TYPE>::Run(pType); \
	}

#define REGISTER_FIELD(DESCRIPTOR, TYPE, FIELD_TYPE, FIELD_NAME, ATTRIBUTE) \
	{ \
		Core::FieldDescriptor* pNewField = DESCRIPTOR->AddField(); \
		Core::FieldInitializer<FIELD_TYPE>::Run(pNewField, #FIELD_NAME, offsetof(TYPE, FIELD_NAME), Core::FieldAttribute(ATTRIBUTE)); \
	}

// Macro to put first before the class and outside any namespace
#define ENABLE_REFLECTION(NAMESPACE, TYPE) \
	namespace NAMESPACE { \
		class TYPE; \
	} \
	DEFINE_TYPE_RESOLVER(NAMESPACE::TYPE)

// Macro to start the description of the reflection
#define START_REFLECTION(TYPE) \
public: \
	static void RegisterReflection() {\
		using ClassType = TYPE; \
		Core::TypeDescriptor* pType = Core::ReflectionMgr::Get().RegisterType<TYPE>(#TYPE); \
		Core::TypeInitializer<TYPE>::Run(pType);
	
// Macro to end the description of the reflection
#define END_REFLECTION() }

// Macro to add a field to the reflection
#define ADD_FIELD(FIELD_NAME) REGISTER_FIELD(pType, ClassType, decltype(FIELD_NAME), FIELD_NAME, Core::None)
#define ADD_FIELD_ATTR(FIELD_NAME, ATTRIBUTE) REGISTER_FIELD(pType, ClassType, decltype(FIELD_NAME), FIELD_NAME, ATTRIBUTE)

// Macro to add a base type to the current type
#define ADD_BASETYPE(BASETYPE) pType->SetBaseType(#BASETYPE);

// Macro the set the upgrade type
#define ADD_UPGRADE_TYPE(TYPE, FUNC) \
	pType->SetUpgradeType(MAKESID(#TYPE)); \
	pType->Upgrade = [](const void* pSrc, void* pDst) { FUNC(static_cast<const ClassType*>(pSrc), static_cast<TYPE*>(pDst)); };

#define ADD_AUTO_UPGRADE(TYPE) \
	pType->SetUpgradeType(MAKESID(#TYPE)); \
	pType->Upgrade = [](const void* pSrc, void* pDst) { static_cast<TYPE*>(pDst)->Upgrade(static_cast<const ClassType*>(pSrc)); };
