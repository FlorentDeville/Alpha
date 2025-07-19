/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/GameObject.h"

namespace Systems
{
	GameObject::GameObject(const std::string& name)
		: Object()
		, m_name(name)
	{
		Systems::CreateObjectInPlace(&m_transform);
	}

	void GameObject::Update()
	{ }

	void GameObject::Render()
	{ }

	void GameObject::SetGuid(const Core::Guid& guid)
	{
		m_guid = guid;
	}

	const Core::Guid& GameObject::GetGuid() const
	{
		return m_guid;
	}

	TransformComponent& GameObject::GetTransform()
	{
		return m_transform;
	}
}
