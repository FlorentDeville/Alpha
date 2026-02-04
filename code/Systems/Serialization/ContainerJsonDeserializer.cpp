/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Serialization/ContainerJsonDeserializer.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonObject.h"
#include "Core/Json/JsonValue.h"
#include "Core/String/BytesToHexa.h"

#include "Systems/Assets/AssetObjects/AssetUtil.h"
#include "Systems/Container/Container.h"
#include "Systems/Serialization/ObjectJsonDeserializer.h"
#include "Systems/Objects/AssetObject.h"

namespace Systems
{
	bool ContainerJsonDeserializer::Deserialize(const Core::JsonObject& jsonObj, Container& pContainer)
	{
		//read the header first
		{
			Core::JsonObject* pJsonHeader = jsonObj.GetMember("header").GetValueAsObject();

			std::string strId = pJsonHeader->GetMember("id").GetValueAsString();
			ContainerId cid(strId.c_str());
			pContainer.SetId(cid);
		}

		//load the hard asset ref
		const Core::JsonValue& jsonValue = jsonObj.GetMember("hard-asset-refs");
		if (!jsonValue.IsNull())
		{
			const Core::JsonArray* pHardAssetRefArray = jsonValue.GetValueAsArray();
			if (pHardAssetRefArray)
			{
				const size_t size = pHardAssetRefArray->GetSize();
				for (int ii = 0; ii < size; ++ii)
				{
					const Core::JsonValue* pValue = pHardAssetRefArray->GetElement(ii);
					const std::string& pValueAsString = pValue->GetValueAsString();
					uint64_t value = Core::HexaToUint64(pValueAsString);
					Systems::NewAssetId id(value);
					AssetUtil::LoadAsset(id);
				}
			}
		}

		//now read the objects
		Core::JsonArray* pJsonArray = jsonObj.GetMember("objects").GetValueAsArray();

		size_t size = pJsonArray->GetSize();
		for (int ii = 0; ii < size; ++ii)
		{
			AssetObject* pAssetObject = nullptr;
			Object** pObject = reinterpret_cast<Object**>(&pAssetObject);
			Core::JsonObject* pJsonObject = pJsonArray->GetElement(ii)->GetValueAsObject();
			ObjectJsonDeserializer deser;
			bool res = deser.Deserialize(*pJsonObject, pObject);
			if (!res)
				return false;

			pContainer.AddAsset(pAssetObject);

			//resolve the hard asset ref. It's ugly but it works.
			const Core::Array<void*>& hardAssetRefArray = deser.GetHardAssetRefArray();
			for (void* pPtr : hardAssetRefArray)
			{
				const NewAssetId* pId = reinterpret_cast<const NewAssetId*>(pPtr);
				if (!pId->IsValid())
					continue;

				uint64_t* pBasePtr = reinterpret_cast<uint64_t*>(pPtr);
				Systems::AssetObject** pAssetPtr = reinterpret_cast<Systems::AssetObject**>(pBasePtr + 1);
				*pAssetPtr = AssetUtil::GetAsset(*pId);
			}
		}

		return true;
	}
}