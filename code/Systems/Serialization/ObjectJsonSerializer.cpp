/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Serialization/ObjectJsonSerializer.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonObject.h"
#include "Core/Json/JsonValue.h"

#include "Core/Collections/BaseArray.h"
#include "Core/Math/Vec4f.h"

#include "Systems/Objects/Object.h"
#include "Systems/Assets/AssetId.h"

namespace Systems
{
	const uint8_t HEADER_VERSION = 1;

	bool SerializeArray(const void* pArrayPtr, const FieldDescriptor* pFieldArray, Core::JsonArray& jsonArray);
	bool SerializeObject(const Object* pObject, const TypeDescriptor* pType, Core::JsonObject& jsonObj);

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

	bool SerializeField(const void* pFieldPtr, const FieldDescriptor* pField, const TypeDescriptor* pDescription, Core::JsonValue& value)
	{
		if (pDescription->GetFields().size() > 0)
		{
			Core::JsonObject* pJsonObject = new Core::JsonObject();
			value.Set(pJsonObject);
			return SerializeObject(reinterpret_cast<const Object*>(pFieldPtr), pDescription, *pJsonObject);
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

		case SID("Systems::AssetId"):
		{
			const Systems::AssetId* pAssetId = reinterpret_cast<const Systems::AssetId*>(pFieldPtr);
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
			//don't support arrays of arrays
			//don't support array of pointers
			assert(pField && !pField->IsElementPointer()); //don't support array of pointers for now

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
		assert(!pFieldArray->IsElementPointer()); //don't support array of pointers for now

		const Core::BaseArray* pArray = reinterpret_cast<const Core::BaseArray*>(pArrayPtr);

		int32_t size = pArray->GetSize();

		for(int ii = 0; ii < size; ++ii)
		{
			const void* item = pArray->GetConstElement(ii);
			Core::JsonValue* pNewValue = new Core::JsonValue();
			jsonArray.AddElement(pNewValue);

			bool res = SerializeField(item, nullptr, pFieldArray->GetElementType(), *pNewValue);
			if (!res)
				return false;
		}

		return true;
	}

	bool SerializeObject(const Object* pObject, const TypeDescriptor* pType, Core::JsonObject& jsonObj)
	{
		const TypeDescriptor* pBaseType = pType->GetBaseType();
		if (pBaseType)
		{
			bool res = SerializeObject(pObject, pBaseType, jsonObj);
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

	bool ObjectJsonSerializer::Serialize(const Object* pObject, Core::JsonObject& jsonObj)
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

			bool res = SerializeObject(pObject, pTypeDescriptor, *pJsonObject);
			return res;

		}
	}
}
