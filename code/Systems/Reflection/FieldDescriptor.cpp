/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Reflection/FieldDescriptor.h"

namespace Systems
{
	FieldDescriptor::FieldDescriptor()
		: m_name()
		, m_offset(0)
		, m_pType(nullptr)
		, m_pElementType(nullptr)
		, m_attribute(FieldAttribute::None)
		, m_isPointer(false)
		, m_isContainer(false)
	{ }

	FieldDescriptor::~FieldDescriptor()
	{ }
}
