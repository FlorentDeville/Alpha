/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Reflection/FieldDescriptor.h"
#include "Systems/Reflection/ReflectionMgr.h"

// Define macros to enable serialization of a class
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

// Macro to specialize the template function GetTypname()
#define DEFINE_GET_TYPENAME(TYPE) template<> inline const std::string& ReflectionMgr::GetTypename<TYPE>() const { static std::string strType = #TYPE; return strType; } 

// Macro to specialize the template function GetTypname() inside the Systems namespace
#define DEFINE_SYSTEMS_GET_TYPENAME(TYPE) \
	OPEN_SYSTEMS_NAMESPACE \
		DEFINE_GET_TYPENAME(TYPE) \
	CLOSE_SYSTEMS_NAMESPACE

// Macro to register a type
#define REGISTER_TYPE(TYPE) Systems::ReflectionMgr::Get().RegisterType<TYPE>(#TYPE)

#define REGISTER_FIELD(DESCRIPTOR, TYPE, FIELD_TYPE, FIELD_NAME) DESCRIPTOR->AddField<FIELD_TYPE>(#FIELD_NAME, offsetof(TYPE, FIELD_NAME), Systems::FieldAttribute::None);

// Macro to put first before the class
#define ENABLE_REFLECTION_WITH_NS(NAMESPACE, TYPE) \
	class TYPE; \
	DEFINE_GET_TYPENAME(NAMESPACE::TYPE)

// Macro to put first before the class
#define ENABLE_REFLECTION(TYPE) \
	class TYPE; \
	DEFINE_GET_TYPENAME(TYPE)

// Macro to put first before the class
#define ENABLE_SYSTEMS_REFLECTION(TYPE) \
	class TYPE; \
	DEFINE_SYSTEMS_GET_TYPENAME(TYPE)

// Macro to start the description of the reflection
#define START_REFLECTION(TYPE) \
public: \
	static void RegisterReflection() {\
		using ClassType = TYPE; \
		Systems::TypeDescriptor* pType = Systems::ReflectionMgr::Get().RegisterType<TYPE>(#TYPE); \
		pType; //prevent warning for empty class.
	
// Macro to end the description of the reflection
#define END_REFLECTION() }

// Macro to add a field to the reflection
#define ADD_FIELD(FIELD_NAME) REGISTER_FIELD(pType, ClassType, decltype(FIELD_NAME), FIELD_NAME)

// Macro to add a base type to the current type
#define ADD_BASETYPE(BASETYPE) pType->SetBaseType(#BASETYPE);
	