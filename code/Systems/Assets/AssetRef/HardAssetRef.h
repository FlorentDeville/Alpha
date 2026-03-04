/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/AssetRef/HardAssetRefRaw.h"
#include "Systems/Assets/NewAssetId.h"
#include "Core/Reflection/FieldDescriptor.h"
#include "Core/Reflection/TypeDescriptor.h"

namespace Systems
{
	// HardAssetRef is a reference to an asset meant to be used in any serialized object.
	// The referenced asset is loaded automatically when the object containing this HardAssetRef is loaded.
	// It is read only and cannot be reset.
	// It is meant to be used like a pointer so you can use operators -> and *.
	template<typename T> class HardAssetRef
	{
	public:
		HardAssetRef();
		HardAssetRef(NewAssetId id);
		~HardAssetRef();

		T* operator->();
		const T* operator->() const;

		T& operator*();
		const T& operator*() const;

		// Get a pointer to the asset if the asset is loaded, otherwise the pointer stays null. 
		// Return true if a valid pointer is found, false otherwise.
		bool Resolve();

		T* GetPtr();
		const T* GetPtr() const;

		static void RegisterFields(Core::TypeDescriptor* pType)
		{
			Core::FieldDescriptor* pIdField = pType->AddField();
			Core::FieldInitializer<decltype(HardAssetRef<T>::m_internalRef.m_id)>::Run(pIdField, "m_id", offsetof(HardAssetRef<T>, m_internalRef.m_id), Core::None);
		}

	private:
		HardAssetRefRaw m_internalRef;
	};

	template<typename T> class Core::TypeInitializer<Systems::HardAssetRef<T>>
	{
	public:
		static void Run(Core::TypeDescriptor* pType)
		{
			pType->m_size = sizeof(Systems::HardAssetRef<T>);
			pType->Construct = []() -> void* { return new Systems::HardAssetRef<T>(); };
			pType->InPlaceConstruct = [](void* ptr) -> void* { return new(ptr) Systems::HardAssetRef<T>(); };
			pType->Destruct = [](void* pObject) { delete reinterpret_cast<Systems::HardAssetRef<T>*>(pObject); };
			pType->Copy = [](const void* pSrc, void* pDst) { *reinterpret_cast<Systems::HardAssetRef<T>*>(pDst) = *reinterpret_cast<const Systems::HardAssetRef<T>*>(pSrc); };

			pType->m_isContainer = false;
			pType->m_isTemplate = true;

			pType->m_sidWithoutTemplateParam = CONSTSID("Systems::HardAssetRef");

			using NonPointerElementType = typename Core::RemovePointer<T>::type;
			pType->m_pTemplateParamType = Core::TypeResolver<NonPointerElementType>::GetType();

			pType->m_isTemplateParamTypePointer = false;

			Systems::HardAssetRef<T>::RegisterFields(pType);
		}
	};

	template<typename T> class Core::FieldInitializer<Systems::HardAssetRef<T>>
	{
	public:
		static void Run(Core::FieldDescriptor* pField, const std::string& name, size_t offset, Core::FieldAttribute attribute)
		{
			typedef Core::RemovePointer<T>::type NonPointerTemplateParamType;
			TypeDescriptor* pTemplateParamType = Core::TypeResolver<NonPointerTemplateParamType>::GetType();

			bool isTemplateParamPointer = Core::IsPointer<T>::value;
			(void)isTemplateParamPointer; //prevent an a warning in release on github.
			assert(!isTemplateParamPointer); // HardAssetRef never use a pointer as template param type.

			std::string templateTypename = "Systems::HardAssetRef<" + pTemplateParamType->GetName() + ">";

			TypeDescriptor* pTemplateType = Core::ReflectionMgr::Get().GetOrAddType(templateTypename);
			if (!pTemplateType->IsInitialized())
			{
				TypeInitializer<Systems::HardAssetRef<T>>::Run(pTemplateType);
			}

			pField->m_name = name;
			pField->m_offset = offset;
			pField->m_pType = pTemplateType;
			pField->m_isPointer = false;
			pField->m_attribute = attribute;
		}
	};
}

#include "Systems/Assets/AssetRef/HardAssetRef.inl"
