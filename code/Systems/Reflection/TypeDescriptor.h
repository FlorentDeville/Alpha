/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

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

	private:
		std::string m_name;
		uint64_t m_id; //should be a typedef. This is a non persistant id. It can be different between execution
		uint64_t m_size;
		const TypeDescriptor* m_pBaseType;
		std::vector<FieldDescriptor> m_fields;

		static uint64_t s_idCounter;
	};
}