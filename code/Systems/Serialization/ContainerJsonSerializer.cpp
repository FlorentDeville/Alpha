/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Serialization/ContainerJsonSerializer.h"

#include "Core/Collections/HashSet.h"
#include "Core/Json/JsonArray.h"
#include "Core/Json/JsonObject.h"

#include "Systems/Container/Container.h"
#include "Systems/Objects/AssetObject.h"
#include "Systems/Serialization/ObjectJsonSerializer.h"

namespace Systems
{
	bool ContainerJsonSerializer::Serialize(const Container* pContainer, Core::JsonObject& pJsonObject)
	{
		//write a container header
		{
			const int CONTAINER_HEADER_VERSION = 1;

			Core::JsonObject* pJsonHeader = pJsonObject.AddObject("header");
			pJsonHeader->AddMember("header-version", CONTAINER_HEADER_VERSION);
			pJsonHeader->AddMember("id", pContainer->GetId().ToString());
		}

		//write the list of hard asset reference
		Core::JsonArray* pJsonHardAssetRefArray = pJsonObject.AddArray("hard-asset-refs");

		//loop through every asset and serialize them
		Core::JsonArray* pJsonObjectArray = pJsonObject.AddArray("objects");
		const std::vector<Systems::AssetObject*>& objects = pContainer->GetAssets();
		for (const Systems::AssetObject* obj : objects)
		{
			Core::JsonObject* pJsonObj = pJsonObjectArray->AddObject();
			ObjectJsonSerializer ser;
			bool res = ser.Serialize(obj, *pJsonObj);
			if (!res)
				return false;

			Core::HashSet<Systems::NewAssetId> uniqueHardAssetRef;

			const Core::Array<Systems::NewAssetId>& hardAssetRefArray = ser.GetHardAssetRefArray();
			for (const Systems::NewAssetId& id : hardAssetRefArray)
			{
				bool alreadyExists = uniqueHardAssetRef.Insert(id);

				if(!alreadyExists)
					pJsonHardAssetRefArray->AddElement(id.ToString());
			}
		}

		return true;
	}
}
