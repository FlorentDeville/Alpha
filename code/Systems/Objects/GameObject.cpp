/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Systems/Objects/GameObject.h"

namespace Systems
{
	GameObject::GameObject(const std::string& name)
		: Object()
		, m_name(name)
	{ }

	void GameObject::Update()
	{ }

	void GameObject::Render()
	{ }
}
