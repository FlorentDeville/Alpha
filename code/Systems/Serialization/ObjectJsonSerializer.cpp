/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Serialization/ObjectJsonSerializer.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonObject.h"
#include "Core/Json/JsonValue.h"

#include "Core/Collections/BaseArray.h"
#include "Core/Math/Vec4f.h"

#include "Systems/Assets/AssetId.h"
#include "Systems/Assets/NewAssetId.h"
#include "Systems/Objects/Object.h"
#include "Systems/Serialization/ObjectHeader.h"

namespace Systems
{
	bool SerializeArray(const void* pArrayPtr, const FieldDescriptor* pFieldArray, Core::JsonArray& jsonArray);
	bool SerializeClass(const void* pObject, const TypeDescriptor* pType, Core::JsonObject& jsonObj);
	bool SerializeObject(const Object* pObject, Core::JsonObject& jsonObj);

	template<typename T> bool SerializeData(const T& data, Core::JsonValue& value)
	{
		value.Set(data);
		return true;
	}

	template<> bool SerializeData<Core::Vec4f>(const Core::Vec4f& data, Core::JsonValue& value)
	{
		Core::JsonArray* pArray = new Core::JsonArray();
		pArray->AddElement(data.GetX());
		pArray->AddElement(data.GetY());
		pArray->AddElement(data.GetZ());
		pArray->AddElement(data.GetW());

		value.Set(pArray);

		return true;
	}

	template<> bool SerializeData<Systems::AssetId>(const Systems::AssetId& data, Core::JsonValue& value)
	{
		value.Set(data.ToString());
		return true;
	}

	template<> bool SerializeData<Systems::NewAssetId>(const Systems::NewAssetId& data, Core::JsonValue& value)
	{
		value.Set(data.ToString());
		return true;
	}

	bool SerializeField(const void* pFieldPtr, const FieldDescriptor* pField, const TypeDescriptor* pDescription, Core::JsonValue& value)
	{
		if (pField && pField->IsPointer())
		{
			const char* pCharPtr = reinterpret_cast<const char*>(pFieldPtr);
			pFieldPtr = reinterpret_cast<const char*>(*pCharPtr);
		}

		if (pDescription->IsObject())
		{
			Core::JsonObject* pJsonObject = new Core::JsonObject();
			value.Set(pJsonObject);
			const Systems::Object* pObject = reinterpret_cast<const Systems::Object*>(pFieldPtr);
			return SerializeObject(pObject, *pJsonObject);
		}
		else if (pDescription->IsClass())
		{
			Core::JsonObject* pJsonObject = new Core::JsonObject();
			value.Set(pJsonObject);
			return SerializeClass(pFieldPtr, pDescription, *pJsonObject);
		}

		switch (pDescription->GetSid())
		{
		case SID("int32_t"):
		{
			const int32_t* pValue = reinterpret_cast<const int32_t*>(pFieldPtr);
			SerializeData(*pValue, value);
		}
		break;
		
		case SID("uint16_t"):
		{
			const uint16_t* pValue = reinterpret_cast<const uint16_t*>(pFieldPtr);
			SerializeData(*pValue, value);
		}
		break;

		case SID("float"):
		{
			const float* pValue = reinterpret_cast<const float*>(pFieldPtr);
			SerializeData(*pValue, value);
		}
		break;

		case SID("Systems::AssetId"):
		{
			const Systems::AssetId* pAssetId = reinterpret_cast<const Systems::AssetId*>(pFieldPtr);
			SerializeData(*pAssetId, value);
		}
		break;

		case SID("Systems::NewAssetId"):
		{
			const Systems::NewAssetId* pAssetId = reinterpret_cast<const Systems::NewAssetId*>(pFieldPtr);
			SerializeData(*pAssetId, value);
		}
		break;

		case SID("std::string"):
		{
			const std::string* pStr = reinterpret_cast<const std::string*>(pFieldPtr);
			SerializeData(*pStr, value);
		}
		break;

		case SID("Core::Vec4f"):
		{
			const Core::Vec4f* pVec = reinterpret_cast<const Core::Vec4f*>(pFieldPtr);
			SerializeData(*pVec, value);
		}
		break;

		case SID("Core::Array"):
		{
			assert(pField); //don't support array of arrays

			const Core::BaseArray* pArray = reinterpret_cast<const Core::BaseArray*>(pFieldPtr);
			Core::JsonArray* pNewArray = new Core::JsonArray();
			value.Set(pNewArray);

			SerializeArray(pFieldPtr, pField, *pNewArray);
		}
		break;

		default:
			assert(false);
			break;
		}

		return true;
	}

	bool SerializeArray(const void* pArrayPtr, const FieldDescriptor* pFieldArray, Core::JsonArray& jsonArray)
	{
		const Core::BaseArray* pArray = reinterpret_cast<const Core::BaseArray*>(pArrayPtr);

		int32_t size = pArray->GetSize();

		for(int ii = 0; ii < size; ++ii)
		{
			const void* pItem = pArray->GetConstElement(ii);
			if (pFieldArray->IsElementPointer())
			{
				const char* pCharPtr = reinterpret_cast<const char*>(pItem);
				pItem = reinterpret_cast<const char*>(*pCharPtr);
			}

			Core::JsonValue* pNewValue = new Core::JsonValue();
			jsonArray.AddElement(pNewValue);

			bool res = SerializeField(pItem, nullptr, pFieldArray->GetElementType(), *pNewValue);
			if (!res)
				return false;
		}

		return true;
	}

	bool SerializeClass(const void* pObject, const TypeDescriptor* pType, Core::JsonObject& jsonObj)
	{
		const TypeDescriptor* pBaseType = pType->GetBaseType();
		if (pBaseType)
		{
			bool res = SerializeClass(pObject, pBaseType, jsonObj);
			if (!res)
				return false;
		}

		const std::vector<Systems::FieldDescriptor>& fieldsArray = pType->GetFields();
		for (const Systems::FieldDescriptor& field : fieldsArray)
		{
			assert(!field.IsPointer()); //don't support pointer for now

			Core::JsonValue* pValue = jsonObj.AddMember(field.GetName());
			const void* pFieldPtr = field.GetDataPtr(pObject);
			SerializeField(pFieldPtr, &field, field.GetType(), *pValue);
		}

		return true;
	}

	bool SerializeObject(const Object* pObject, Core::JsonObject& jsonObj)
	{
		const Systems::TypeDescriptor* pTypeDescriptor = pObject->GetTypeDescriptor();
		if (!pTypeDescriptor)
			return false;

		//first write an object header
		{
			Core::JsonObject* pHeader = jsonObj.AddObject("header");
			pHeader->AddMember("header-version", HEADER_VERSION);
			pHeader->AddMember("typename", pTypeDescriptor->GetName());
		}

		//now write the object itself
		{
			Core::JsonObject* pJsonObject = jsonObj.AddObject("object");

			bool res = SerializeClass(pObject, pTypeDescriptor, *pJsonObject);
			return res;

		}
	}

	bool ObjectJsonSerializer::Serialize(const Object* pObject, Core::JsonObject& jsonObj)
	{
		return SerializeObject(pObject, jsonObj);
	}
}
