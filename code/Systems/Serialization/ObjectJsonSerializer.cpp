/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Serialization/ObjectJsonSerializer.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonObject.h"
#include "Core/Json/JsonValue.h"

#include "Core/Collections/BaseArray.h"
#include "Core/Guid/Guid.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"
#include "Core/String/BytesToHexa.h"

#include "Systems/Assets/NewAssetId.h"
#include "Systems/Objects/Object.h"
#include "Systems/Serialization/ObjectHeader.h"

namespace Systems
{
	template<typename T> bool SerializeData(const T& data, Core::JsonValue& value)
	{
		value.Set(data);
		return true;
	}

	template<> bool SerializeData<Core::Guid>(const Core::Guid& data, Core::JsonValue& value)
	{
		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };
		data.ToString(buffer, BUFFER_SIZE);

		value.Set(buffer);
		return true;
	}

	template<> bool SerializeData<Core::Mat44f>(const Core::Mat44f& data, Core::JsonValue& value)
	{
		Core::JsonArray* pArray = new Core::JsonArray();
		for (int ii = 0; ii < 4; ++ii)
		{
			for (int jj = 0; jj < 4; ++jj)
			{
				pArray->AddElement(data.Get(ii, jj));
			}
		}

		value.Set(pArray);
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

	template<> bool SerializeData<Systems::NewAssetId>(const Systems::NewAssetId& data, Core::JsonValue& value)
	{
		value.Set(data.ToString());
		return true;
	}

	bool ObjectJsonSerializer::SerializeField(const void* pFieldPtr, const FieldDescriptor* pField, const TypeDescriptor* pDescription, Core::JsonValue& value)
	{
		if (pField && pField->IsPointer())
		{
			const char* pCharPtr = reinterpret_cast<const char*>(pFieldPtr);
			pFieldPtr = reinterpret_cast<const char*>(*pCharPtr);
		}

		if (pDescription->IsContainer())
		{
			const Core::BaseArray* pArray = reinterpret_cast<const Core::BaseArray*>(pFieldPtr);
			Core::JsonArray* pNewArray = new Core::JsonArray();
			value.Set(pNewArray);

			return SerializeArray(pFieldPtr, pField, *pNewArray);
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
			//save the list of HardAssetRef
			if (pDescription->GetSidWithoutTemplateParam() == CONSTSID("Systems::HardAssetRef"))
			{
				const std::vector<FieldDescriptor*>& fields = pDescription->GetFields();
				const FieldDescriptor* pFieldId = fields[0];
				const Systems::NewAssetId* pAssetId = pFieldId->GetDataPtr<Systems::NewAssetId>(pFieldPtr);
				m_hardAssetRefArray.PushBack(*pAssetId);
			}

			Core::JsonObject* pJsonObject = new Core::JsonObject();
			value.Set(pJsonObject);
			return SerializeClass(pFieldPtr, pDescription, *pJsonObject);
		}

		switch (pDescription->GetSid())
		{
		case SID("int8_t"):
		{
			const int8_t* pValue = reinterpret_cast<const int8_t*>(pFieldPtr);
			SerializeData(*pValue, value);
		}
		break;

		case SID("int32_t"):
		{
			const int32_t* pValue = reinterpret_cast<const int32_t*>(pFieldPtr);
			SerializeData(*pValue, value);
		}
		break;

		case SID("uint32_t"):
		{
			const uint32_t* pValue = reinterpret_cast<const uint32_t*>(pFieldPtr);
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

		case SID("char"):
		{
			const char* pValue = reinterpret_cast<const char*>(pFieldPtr);
			SerializeData(*pValue, value);
		}
		break;

		case SID("bool"):
		{
			const bool* pValue = reinterpret_cast<const bool*>(pFieldPtr);
			SerializeData(*pValue, value);
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

		case SID("Core::Guid"):
		{
			const Core::Guid* pGuid = reinterpret_cast<const Core::Guid*>(pFieldPtr);
			SerializeData(*pGuid, value);
		}
		break;

		case SID("Core::Mat44f"):
		{
			const Core::Mat44f* pMat = reinterpret_cast<const Core::Mat44f*>(pFieldPtr);
			SerializeData(*pMat, value);
		}
		break;

		case SID("Core::Vec4f"):
		{
			const Core::Vec4f* pVec = reinterpret_cast<const Core::Vec4f*>(pFieldPtr);
			SerializeData(*pVec, value);
		}
		break;

		case SID("Core::Sid"):
		{
			const Core::Sid* pSid = reinterpret_cast<const Core::Sid*>(pFieldPtr);
			std::string strSid = Core::Uint64ToHexa(*pSid);
			SerializeData(strSid, value);
		}
		break;

		default:
			assert(false);
			break;
		}

		return true;
	}

	bool ObjectJsonSerializer::SerializeArray(const void* pArrayPtr, const FieldDescriptor* pFieldArray, Core::JsonArray& jsonArray)
	{
		const Core::BaseArray* pArray = reinterpret_cast<const Core::BaseArray*>(pArrayPtr);
		const TypeDescriptor* pArrayType = pFieldArray->GetType();

		int32_t size = pArray->GetSize();

		for(int ii = 0; ii < size; ++ii)
		{
			const void* pItem = pArray->GetConstElement(ii);
			if (pArrayType->IsTemplateParamTypePointer())
			{
				const uint64_t* pPtr = reinterpret_cast<const uint64_t*>(pItem);
				pItem = reinterpret_cast<const void*>(*pPtr);
			}

			Core::JsonValue* pNewValue = new Core::JsonValue();
			jsonArray.AddElement(pNewValue);

			bool res = SerializeField(pItem, nullptr, pArrayType->GetTemplateParamType(), *pNewValue);
			if (!res)
				return false;
		}

		return true;
	}

	bool ObjectJsonSerializer::SerializeClass(const void* pObject, const TypeDescriptor* pType, Core::JsonObject& jsonObj)
	{
		const TypeDescriptor* pBaseType = pType->GetBaseType();
		if (pBaseType)
		{
			bool res = SerializeClass(pObject, pBaseType, jsonObj);
			if (!res)
				return false;
		}

		const std::vector<Systems::FieldDescriptor*>& fieldsArray = pType->GetFields();
		for (const Systems::FieldDescriptor* pField : fieldsArray)
		{
			assert(!pField->IsPointer()); //don't support pointer for now

			Core::JsonValue* pValue = jsonObj.AddMember(pField->GetName());
			const void* pFieldPtr = pField->GetDataPtr(pObject);
			SerializeField(pFieldPtr, pField, pField->GetType(), *pValue);
		}

		return true;
	}

	bool ObjectJsonSerializer::SerializeObject(const Object* pObject, Core::JsonObject& jsonObj)
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

	const Core::Array<Systems::NewAssetId>& ObjectJsonSerializer::GetHardAssetRefArray() const
	{
		return m_hardAssetRefArray;
	}
}
