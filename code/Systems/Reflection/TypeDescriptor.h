/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Sid/Sid.h"
#include "Systems/Reflection/FieldAttribute.h"

#include <string>
#include <vector>

namespace Systems
{
	class FieldDescriptor;

	class TypeDescriptor
	{
	public:
		TypeDescriptor(const std::string& name, uint64_t size);

		FieldDescriptor* AddField();

		void SetBaseType(const std::string& baseTypeName);

		const std::string& GetName() const;
		Core::Sid GetSid() const;
		const std::vector<FieldDescriptor>& GetFields() const;
		const TypeDescriptor* GetBaseType() const;

	private:
		std::string m_name;
		Core::Sid m_sid; //sid of m_name. It is deterministic, can be serialized and used to compare types.
		uint64_t m_size;
		const TypeDescriptor* m_pBaseType;
		std::vector<FieldDescriptor> m_fields;
	};
}