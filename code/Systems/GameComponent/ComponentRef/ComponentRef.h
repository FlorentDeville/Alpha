/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Reflection/FieldDescriptor.h"
#include "Core/Reflection/TypeDescriptor.h"

#include "Systems/GameComponent/ComponentRef/ComponentRefRaw.h"

namespace Systems
{
	template<typename T> class ComponentRef
	{
	public:
		ComponentRef();
		ComponentRef(const Core::Guid& guid);
		~ComponentRef();

		static void RegisterFields(Core::TypeDescriptor* pType)
		{
			Core::FieldDescriptor* pIdField = pType->AddField();
			Core::FieldInitializer<decltype(ComponentRef<T>::m_internalRef.m_guid)>::Run(pIdField, "m_guid", offsetof(ComponentRef<T>, m_internalRef.m_guid), Core::None);
		}

	private:
		ComponentRefRaw m_internalRef;
	};

	template<typename T> class Core::TypeInitializer<Systems::ComponentRef<T>>
	{
	public:
		static void Run(Core::TypeDescriptor* pType)
		{
			pType->m_size = sizeof(Systems::ComponentRef<T>);
			pType->Construct = []() -> void* { return new Systems::ComponentRef<T>(); };
			pType->InPlaceConstruct = [](void* ptr) -> void* { return new(ptr) Systems::ComponentRef<T>(); };
			pType->Destruct = [](void* pObject) { delete reinterpret_cast<Systems::ComponentRef<T>*>(pObject); };
			pType->Copy = [](const void* pSrc, void* pDst) { *reinterpret_cast<Systems::ComponentRef<T>*>(pDst) = *reinterpret_cast<const Systems::ComponentRef<T>*>(pSrc); };

			pType->m_isContainer = false;
			pType->m_isTemplate = true;

			pType->m_sidWithoutTemplateParam = CONSTSID("Systems::ComponentRef");

			using NonPointerElementType = typename Core::RemovePointer<T>::type;
			pType->m_pTemplateParamType = Core::TypeResolver<NonPointerElementType>::GetType();

			pType->m_isTemplateParamTypePointer = false;

			Systems::ComponentRef<T>::RegisterFields(pType);
		}
	};

	template<typename T> class Core::FieldInitializer<Systems::ComponentRef<T>>
	{
	public:
		static void Run(Core::FieldDescriptor* pField, const std::string& name, size_t offset, Core::FieldAttribute attribute)
		{
			typedef Core::RemovePointer<T>::type NonPointerTemplateParamType;
			TypeDescriptor* pTemplateParamType = Core::TypeResolver<NonPointerTemplateParamType>::GetType();

			bool isTemplateParamPointer = Core::IsPointer<T>::value;
			(void)isTemplateParamPointer; //prevent an a warning in release on github.
			assert(!isTemplateParamPointer); // HardAssetRef never use a pointer as template param type.

			std::string templateTypename = "Systems::ComponentRef<" + pTemplateParamType->GetName() + ">";

			TypeDescriptor* pTemplateType = Core::ReflectionMgr::Get().GetOrAddType(templateTypename);
			if (!pTemplateType->IsInitialized())
			{
				TypeInitializer<Systems::ComponentRef<T>>::Run(pTemplateType);
			}

			pField->m_name = name;
			pField->m_offset = offset;
			pField->m_pType = pTemplateType;
			pField->m_isPointer = false;
			pField->m_attribute = attribute;
		}
	};
}

#include "Systems/GameComponent/ComponentRef/ComponentRef.inl"
