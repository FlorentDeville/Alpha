/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Serialization/ObjectJsonDeserializer.h"

#include "Core/Json/JsonMember.h"
#include "Core/Json/JsonObject.h"

#include "Systems/Reflection/ReflectionMgr.h"
#include "Systems/Serialization/ObjectHeader.h"

#include <assert.h>

namespace Systems
{
	bool ObjectJsonDeserializer::Deserialize(const Core::JsonObject& jsonObject, Object** ppObject)
	{
		const std::vector<Core::JsonMember*>& members = jsonObject.GetMembers();

		//read the header first
		std::string objectTypename;
		{
			const Core::JsonMember* pMember = members[0];
			const Core::JsonValue& pJsonHeaderValue = pMember->GetConstValue();

			const Core::JsonObject* pHeader = pJsonHeaderValue.GetValueAsObject();

			const std::vector<Core::JsonMember*>& pHeaderMembers = pHeader->GetMembers();

			uint8_t headerVersion = pHeaderMembers[0]->GetConstValue().GetValueAsDouble();
			assert(headerVersion == HEADER_VERSION);

			objectTypename = pHeaderMembers[1]->GetConstValue().GetValueAsString();
		}

		//now create the object
		TypeDescriptor* pType = ReflectionMgr::Get().GetType(objectTypename);

		return true;
	}
}
