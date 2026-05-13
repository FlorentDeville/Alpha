/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

namespace Systems
{
	template<typename T> ComponentRef<T>::ComponentRef()
		: m_internalRef()
	{ }

	template<typename T> ComponentRef<T>::ComponentRef(const Core::Guid& guid)
		: m_internalRef(guid)
	{ }

	template<typename T> ComponentRef<T>::~ComponentRef()
	{ }
}
