/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Serialization/ContainerJsonDeserializer.h"

#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonObject.h"
#include "Core/Json/JsonValue.h"

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
		}

		return true;
	}
}