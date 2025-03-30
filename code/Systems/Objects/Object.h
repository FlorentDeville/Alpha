/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

namespace Systems
{
	class TypeDescriptor;

	// Base class for any object editable and serializable.
	class Object
	{
	public:
		Object() = default;
		Object() = default;

	private:
		TypeDescriptor* m_typeDescriptor;
	};
}
