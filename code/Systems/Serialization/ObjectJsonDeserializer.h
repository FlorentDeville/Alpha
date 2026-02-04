/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"

namespace Core
{
	class BaseArray;
	class JsonArray;
	class JsonObject;
	class JsonValue;
}

namespace Systems
{
	class FieldDescriptor;
	class Object;
	class TypeDescriptor;

	class ObjectJsonDeserializer
	{
	public:
		ObjectJsonDeserializer() = default;
		~ObjectJsonDeserializer() = default;

		bool Deserialize(const Core::JsonObject& jsonObject, Object** ppObject);

		const Core::Array<void*>& GetHardAssetRefArray() const;

	private:
		Core::Array<void*> m_hardAssetRefArray;

		bool DeserializeArray(const Core::JsonArray& jsonArray, const TypeDescriptor* pElementType, bool elementIsPointer, Core::BaseArray& array);
		bool DeserializeField(const Core::JsonValue& jsonFieldValue, const TypeDescriptor* pFieldType, const FieldDescriptor* pFieldDescriptor, void* pFieldPtr, bool isPointer);
		bool DeserializeObject(const Core::JsonObject* jsonObject, Object** ppObject, bool constructInPlace);
		bool DeserializeClass(const Core::JsonObject* jsonObject, const TypeDescriptor* pType, void* pObject);
	};
}
