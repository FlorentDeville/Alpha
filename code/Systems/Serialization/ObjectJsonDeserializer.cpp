/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Serialization/ObjectJsonDeserializer.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonMember.h"
#include "Core/Json/JsonObject.h"
#include "Core/Math/Vec4f.h"

#include "Systems/Assets/AssetId.h"
#include "Systems/Assets/NewAssetId.h"
#include "Systems/Objects/Object.h"
#include "Systems/Reflection/ReflectionMgr.h"
#include "Systems/Serialization/ObjectHeader.h"

#include <assert.h>
#include <sstream>

namespace Systems
{
	bool DeserializeClass(const Core::JsonObject* jsonObject, const TypeDescriptor* pType, void* pObject);
	bool DeserializeField(const Core::JsonValue& jsonFieldValue, const TypeDescriptor* pFieldType, const FieldDescriptor* pFieldDescriptor, void* pFieldPtr);

	bool DeserializeArray(const Core::JsonArray& jsonArray, const TypeDescriptor* pElementType, Core::BaseArray& array)
	{
		size_t size = jsonArray.GetSize();
		array.Resize(static_cast<int32_t>(size));

		for (int ii = 0; ii < size; ++ii)
		{
			const Core::JsonValue* pJsonValue = jsonArray.GetElement(ii);
			void* pFieldPtr = array.GetElement(ii);
			bool res = DeserializeField(*pJsonValue, pElementType, nullptr, pFieldPtr);
			if (!res)
				return res;
		}

		return true;
	}

	bool DeserializeField(const Core::JsonValue& jsonFieldValue, const TypeDescriptor* pFieldType, const FieldDescriptor* pFieldDescriptor, void* pFieldPtr)
	{
		//Watch out : pFieldDescriptor can be null if this is a field from an array
		void* ptr = pFieldPtr;
		if (pFieldDescriptor && pFieldDescriptor->IsPointer())
		{
			char* pCharPtr = reinterpret_cast<char*>(ptr);
			void* ptr = reinterpret_cast<char*>(*pCharPtr);

			//if the pointer is null, the default constructor didn't created it.
			if (!ptr)
			{
				ptr = pFieldType->Construct();
			}
		}

		if (pFieldType->IsObject())
		{
			Object* pFieldObject = reinterpret_cast<Object*>(ptr);
			pFieldObject->SetTypeDescriptor(pFieldType);

			Core::JsonObject* pJsonFieldObject = jsonFieldValue.GetValueAsObject()->GetMember(1).GetValueAsObject();
			bool res = DeserializeClass(pJsonFieldObject, pFieldType, ptr);
			if (!res)
				return false;

			pFieldObject->PostLoad();
		}
		else if (pFieldType->IsClass())
		{
			return DeserializeClass(jsonFieldValue.GetValueAsObject(), pFieldType, ptr);
		}

		switch (pFieldType->GetSid())
		{
		case SID("int32_t"):
		{
			int32_t* pValue = reinterpret_cast<int32_t*>(ptr);
			*pValue = static_cast<int32_t>(jsonFieldValue.GetValueAsDouble());
		}
		break;

		case SID("uint16_t"):
		{
			uint16_t* pValue = reinterpret_cast<uint16_t*>(ptr);
			*pValue = static_cast<uint16_t>(jsonFieldValue.GetValueAsDouble());
		}
		break;

		case SID("Systems::AssetId"):
		{
			Systems::AssetId* pValue = reinterpret_cast<Systems::AssetId*>(ptr);

			std::string valueStr = jsonFieldValue.GetValueAsString();
			std::stringstream ss(valueStr);
			size_t value = 0;
			ss >> value;

			new (pValue) Systems::AssetId(value);
		}
		break;

		case SID("Systems::NewAssetId"):
		{
			Systems::NewAssetId* pValue = reinterpret_cast<Systems::NewAssetId*>(ptr);

			std::string valueStr = jsonFieldValue.GetValueAsString();
			std::stringstream ss(valueStr);
			size_t value = 0;
			ss >> value;

			new (pValue) Systems::AssetId(value);
		}
		break;

		case SID("std::string"):
		{
			std::string* pValue = reinterpret_cast<std::string*>(ptr);
			pValue->assign(jsonFieldValue.GetValueAsString());
		}
		break;

		case SID("Core::Vec4f"):
		{
			Core::Vec4f* pValue = reinterpret_cast<Core::Vec4f*>(ptr);
			Core::JsonArray* pJsonArray = jsonFieldValue.GetValueAsArray();

			for (int ii = 0; ii < 4; ++ii)
				pValue->Set(ii, static_cast<float>(pJsonArray->GetElement(ii)->GetValueAsDouble()));
		}
		break;

		case SID("Core::Array"):
		{
			//assert(pFieldDescriptor); //don't support array of arrays

			Core::BaseArray* pArray = reinterpret_cast<Core::BaseArray*>(pFieldPtr);
			bool res = DeserializeArray(*jsonFieldValue.GetValueAsArray(), pFieldDescriptor->GetElementType(), *pArray);
			if (!res)
				return false;
		}
		break;

		default:
			assert(false);
			break;
		}

		return true;
	}

	bool DeserializeObject(const Core::JsonObject* jsonObject, Object** ppObject)
	{
		const std::vector<Core::JsonMember*>& members = jsonObject->GetMembers();

		//read the header first
		std::string objectTypename;
		{
			const Core::JsonMember* pMember = members[0];
			const Core::JsonValue& pJsonHeaderValue = pMember->GetConstValue();

			const Core::JsonObject* pHeader = pJsonHeaderValue.GetValueAsObject();

			const std::vector<Core::JsonMember*>& pHeaderMembers = pHeader->GetMembers();

			uint8_t headerVersion = static_cast<uint8_t>(pHeaderMembers[0]->GetConstValue().GetValueAsDouble());
			assert(headerVersion == HEADER_VERSION);

			objectTypename = pHeaderMembers[1]->GetConstValue().GetValueAsString();
		}

		//now create the object
		const TypeDescriptor* pType = ReflectionMgr::Get().GetType(objectTypename);
		*ppObject = reinterpret_cast<Object*>(pType->Construct());
		(*ppObject)->SetTypeDescriptor(pType);

		bool res = DeserializeClass(members[1]->GetConstValue().GetValueAsObject(), pType, *ppObject);
		if (!res)
			return false;

		(*ppObject)->PostLoad();

		//upgrade the type if necessary
		Core::Sid upgradeType = pType->GetUpgradeType();
		if (upgradeType != Core::INVALID_SID)
		{
			//TypeDescriptor* pUpgradeType = ReflectionMgr::Get().GetType()
		}

		return true;
	}

	bool DeserializeClass(const Core::JsonObject* jsonObject, const TypeDescriptor* pType, void* pObject)
	{
		const std::vector<FieldDescriptor>& fields = pType->GetFields();
		for (const FieldDescriptor& field : fields)
		{
			const Core::JsonValue& jsonValue = jsonObject->GetMember(field.GetName());
			void* pFieldPtr = field.GetDataPtr(pObject);
			bool res = DeserializeField(jsonValue, field.GetType(), &field, pFieldPtr);
			if (!res)
				return false;
		}

		return true;
	}

	bool ObjectJsonDeserializer::Deserialize(const Core::JsonObject& jsonObject, Object** ppObject)
	{
		return DeserializeObject(&jsonObject, ppObject);
	}
}
