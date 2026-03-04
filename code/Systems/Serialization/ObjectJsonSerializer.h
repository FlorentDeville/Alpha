/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

namespace Core
{
	class FieldDescriptor;
	class JsonArray;
	class JsonObject;
	class JsonValue;
	class TypeDescriptor;
}

namespace Systems
{
	class NewAssetId;
	class Object;
	
	// Class to serialize an object in json using the reflection
	class ObjectJsonSerializer
	{
	public:
		ObjectJsonSerializer() = default;
		~ObjectJsonSerializer() = default;

		bool Serialize(const Object* pObject, Core::JsonObject& jsonObj);

		const Core::Array<Systems::NewAssetId>& GetHardAssetRefArray() const;

	private:
		Core::Array<Systems::NewAssetId> m_hardAssetRefArray;

		bool SerializeField(const void* pFieldPtr, const Core::FieldDescriptor* pField, const Core::TypeDescriptor* pDescription, Core::JsonValue& value);
		bool SerializeArray(const void* pArrayPtr, const Core::FieldDescriptor* pFieldArray, Core::JsonArray& jsonArray);
		bool SerializeClass(const void* pObject, const Core::TypeDescriptor* pType, Core::JsonObject& jsonObj);
		bool SerializeObject(const Object* pObject, Core::JsonObject& jsonObj);
		bool SerializeEnum(const void* pFieldPtr, const Core::TypeDescriptor* pDescription, Core::JsonValue& value);
	};
}
