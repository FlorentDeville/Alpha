/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Objects/GameObject.h"

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

	template<typename T> T* ComponentRef<T>::FindComponent(GameObject* pObj)
	{
		GameComponent* pComponent = pObj->FindComponent(m_internalRef.GetGuid());
		return static_cast<T*>(pComponent);
	}
}
