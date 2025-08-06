/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Serialization/ObjectJsonDeserializer.h"

#include "Core/Guid/Guid.h"
#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonMember.h"
#include "Core/Json/JsonObject.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"
#include "Core/String/BytesToHexa.h"

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
	bool DeserializeField(const Core::JsonValue& jsonFieldValue, const TypeDescriptor* pFieldType, const FieldDescriptor* pFieldDescriptor, void* pFieldPtr, bool isPointer);
	bool DeserializeObject(const Core::JsonObject* jsonObject, Object** ppObject, bool constructInPlace);

	bool DeserializeArray(const Core::JsonArray& jsonArray, const TypeDescriptor* pElementType, bool elementIsPointer, Core::BaseArray& array)
	{
		size_t size = jsonArray.GetSize();
		array.Resize(static_cast<int32_t>(size));

		for (int ii = 0; ii < size; ++ii)
		{
			const Core::JsonValue* pJsonValue = jsonArray.GetElement(ii);
			void* pFieldPtr = array.GetElement(ii);

			// if this is an array of pointers then I need to allocate the elements.
			if (elementIsPointer && !pElementType->IsObject())
			{
				void* pAllocatedElement = pElementType->Construct();
				uint64_t* pp = reinterpret_cast<uint64_t*>(pFieldPtr);
				*pp = reinterpret_cast<uint64_t>(pAllocatedElement);

				pFieldPtr = pAllocatedElement;
			}

			bool res = DeserializeField(*pJsonValue, pElementType, nullptr, pFieldPtr, elementIsPointer);
			if (!res)
				return res;
		}

		return true;
	}

	bool DeserializeField(const Core::JsonValue& jsonFieldValue, const TypeDescriptor* pFieldType, const FieldDescriptor* pFieldDescriptor, void* pFieldPtr, bool isPointer)
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

		if (pFieldType->IsContainer())
		{
			Core::BaseArray* pArray = reinterpret_cast<Core::BaseArray*>(pFieldPtr);
			bool res = DeserializeArray(*jsonFieldValue.GetValueAsArray(), pFieldType->GetElementType(), pFieldType->IsElementPointer(), *pArray);
			return res;
		}
		else if (pFieldType->IsObject())
		{
			Object* pObject = reinterpret_cast<Object*>(pFieldPtr);
			Object** ppObject = &pObject;

			bool constructInPlace = false;
			if (isPointer)
			{
				ppObject = reinterpret_cast<Object**>(pFieldPtr);
			}
			else
			{
				constructInPlace = true;
			}

			Core::JsonObject* pJsonFieldObject = jsonFieldValue.GetValueAsObject();
			bool res = DeserializeObject(pJsonFieldObject, ppObject, constructInPlace);
			if (!res)
				return false;

			(*ppObject)->PostLoad();

			return true;
		}
		else if (pFieldType->IsClass())
		{
			return DeserializeClass(jsonFieldValue.GetValueAsObject(), pFieldType, ptr);
		}

		switch (pFieldType->GetSid())
		{
		case SID("int8_t"):
		{
			int8_t* pValue = reinterpret_cast<int8_t*>(ptr);
			*pValue = static_cast<int8_t>(jsonFieldValue.GetValueAsDouble());
		}
		break;

		case SID("int32_t"):
		{
			int32_t* pValue = reinterpret_cast<int32_t*>(ptr);
			*pValue = static_cast<int32_t>(jsonFieldValue.GetValueAsDouble());
		}
		break;

		case SID("uint32_t"):
		{
			uint32_t* pValue = reinterpret_cast<uint32_t*>(ptr);
			*pValue = static_cast<uint32_t>(jsonFieldValue.GetValueAsDouble());
		}
		break;

		case SID("uint16_t"):
		{
			uint16_t* pValue = reinterpret_cast<uint16_t*>(ptr);
			*pValue = static_cast<uint16_t>(jsonFieldValue.GetValueAsDouble());
		}
		break;

		case SID("float"):
		{
			float* pValue = reinterpret_cast<float*>(ptr);
			*pValue = static_cast<float>(jsonFieldValue.GetValueAsDouble());
		}
		break;

		case SID("char"):
		{
			char* pValue = reinterpret_cast<char*>(ptr);
			*pValue = static_cast<char>(jsonFieldValue.GetValueAsDouble());
		}
		break;

		case SID("bool"):
		{
			bool* pValue = reinterpret_cast<bool*>(ptr);
			*pValue = static_cast<char>(jsonFieldValue.GetValueAsBool());
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
			uint64_t value = Core::HexaToUint64(valueStr);

			new (pValue) Systems::AssetId(value);
		}
		break;

		case SID("std::string"):
		{
			std::string* pValue = reinterpret_cast<std::string*>(ptr);
			pValue->assign(jsonFieldValue.GetValueAsString());
		}
		break;

		case SID("Core::Guid"):
		{
			Core::Guid* pValue = reinterpret_cast<Core::Guid*>(ptr);
			new(pValue) Core::Guid(jsonFieldValue.GetValueAsString().c_str());
		}
		break;

		case SID("Core::Mat44f"):
		{
			Core::Mat44f* pMat = reinterpret_cast<Core::Mat44f*>(ptr);
			Core::JsonArray* pJsonArray = jsonFieldValue.GetValueAsArray();

			for (int ii = 0; ii < 4; ++ii)
			{
				for (int jj = 0; jj < 4; ++jj)
				{
					float fValue = static_cast<float>(pJsonArray->GetElement(ii * 4 + jj)->GetValueAsDouble());
					pMat->Set(ii, jj, fValue);
				}
			}
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

		case SID("Core::Sid"):
		{
			Core::Sid* pValue = reinterpret_cast<Core::Sid*>(ptr);

			const std::string& strSid = jsonFieldValue.GetValueAsString();
			*pValue = Core::HexaToUint64(strSid);
		}
		break;

		default:
			assert(false);
			break;
		}

		return true;
	}

	bool DeserializeObject(const Core::JsonObject* jsonObject, Object** ppObject, bool constructInPlace)
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

		if (constructInPlace)
		{
			pType->InPlaceConstruct(*ppObject);
			(*ppObject)->SetTypeDescriptor(pType);
		}
		else
		{
			*ppObject = reinterpret_cast<Object*>(pType->Construct());
			(*ppObject)->SetTypeDescriptor(pType);
		}

		bool res = DeserializeClass(members[1]->GetConstValue().GetValueAsObject(), pType, *ppObject);
		if (!res)
			return false;

		(*ppObject)->PostLoad();

		//upgrade the type if necessary
		Core::Sid upgradeType = pType->GetUpgradeType();
		if (upgradeType != Core::INVALID_SID)
		{
			const TypeDescriptor* pUpgradeType = ReflectionMgr::Get().GetType(upgradeType);
			Object* pUpgradeObject = reinterpret_cast<Object*>(pUpgradeType->Construct());
			pUpgradeObject->SetTypeDescriptor(pUpgradeType);

			pType->Upgrade(*ppObject, pUpgradeObject);

			pType->Destruct(*ppObject);
			*ppObject = pUpgradeObject;
		}

		return true;
	}

	bool DeserializeClass(const Core::JsonObject* jsonObject, const TypeDescriptor* pType, void* pObject)
	{
		const TypeDescriptor* pBaseType = pType->GetBaseType();
		if (pBaseType)
		{
			bool res = DeserializeClass(jsonObject, pBaseType, pObject);
			if (!res)
				return false;
		}

		const std::vector<FieldDescriptor*>& fields = pType->GetFields();
		for (const FieldDescriptor* pField : fields)
		{
			const Core::JsonValue& jsonValue = jsonObject->GetMember(pField->GetName());
			if (jsonValue.IsNull())
				continue;

			void* pFieldPtr = pField->GetDataPtr(pObject);
			bool res = DeserializeField(jsonValue, pField->GetType(), pField, pFieldPtr, pField->IsPointer());
			if (!res)
				return false;
		}

		return true;
	}

	bool ObjectJsonDeserializer::Deserialize(const Core::JsonObject& jsonObject, Object** ppObject)
	{
		return DeserializeObject(&jsonObject, ppObject, false);
	}
}
