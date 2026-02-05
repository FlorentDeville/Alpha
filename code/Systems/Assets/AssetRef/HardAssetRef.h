/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Systems/Assets/AssetRef/HardAssetRefRaw.h"
#include "Systems/Assets/NewAssetId.h"
#include "Systems/Reflection/FieldDescriptor.h"
#include "Systems/Reflection/TypeDescriptor.h"

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

		static void RegisterFields(TypeDescriptor* pType)
		{
			Systems::FieldDescriptor* pIdField = pType->AddField();
			Systems::FieldInitializer<decltype(HardAssetRef<T>::m_internalRef.m_id)>::Run(pIdField, "m_id", offsetof(HardAssetRef<T>, m_internalRef.m_id), Systems::FieldAttribute());
		}

	private:
		HardAssetRefRaw m_internalRef;
	};

	template<typename T> class TypeInitializer<Systems::HardAssetRef<T>>
	{
	public:
		static void Run(TypeDescriptor* pType)
		{
			pType->m_size = sizeof(HardAssetRef<T>);
			pType->Construct = []() -> void* { return new HardAssetRef<T>(); };
			pType->InPlaceConstruct = [](void* ptr) -> void* { return new(ptr) HardAssetRef<T>(); };
			pType->Destruct = [](void* pObject) { delete reinterpret_cast<HardAssetRef<T>*>(pObject); };
			pType->Copy = [](const void* pSrc, void* pDst) { *reinterpret_cast<HardAssetRef<T>*>(pDst) = *reinterpret_cast<const HardAssetRef<T>*>(pSrc); };

			pType->m_isContainer = false;
			pType->m_isTemplate = true;

			pType->m_sidWithoutTemplateParam = CONSTSID("Systems::HardAssetRef");

			using NonPointerElementType = typename RemovePointer<T>::type;
			pType->m_pTemplateParamType = TypeResolver<NonPointerElementType>::GetType();

			pType->m_isTemplateParamTypePointer = false;

			HardAssetRef<T>::RegisterFields(pType);
		}
	};

	template<typename T> class FieldInitializer<Systems::HardAssetRef<T>>
	{
	public:
		static void Run(FieldDescriptor* pField, const std::string& name, size_t offset, FieldAttribute attribute)
		{
			typedef RemovePointer<T>::type NonPointerTemplateParamType;
			TypeDescriptor* pTemplateParamType = TypeResolver<NonPointerTemplateParamType>::GetType();

			bool isTemplateParamPointer = IsPointer<T>::value;
			assert(!isTemplateParamPointer); // HardAssetRef never use a pointer as template param type.

			std::string templateTypename = "Systems::HardAssetRef<" + pTemplateParamType->GetName() + ">";

			TypeDescriptor* pTemplateType = Systems::ReflectionMgr::Get().GetOrAddType(templateTypename);
			if (!pTemplateType->IsInitialized())
			{
				TypeInitializer<HardAssetRef<T>>::Run(pTemplateType);
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
