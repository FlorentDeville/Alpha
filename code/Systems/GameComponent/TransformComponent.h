/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Sqt.h"

#include "Systems/Objects/GameComponent.h"
#include "Systems/Reflection/ReflectionCoreTypes.h"

ENABLE_REFLECTION(Systems, TransformComponent)

namespace Core
{
	class Mat44f;
}

namespace Systems
{
	class GameObject;

	class TransformComponent : public GameComponent
	{
	public:
		TransformComponent();
		~TransformComponent();

		const Core::Mat44f& GetLocalTx() const;
		const Core::Mat44f& GetWorldTx() const;

		const Core::Mat44f& GetParentWorldTx() const;

		void SetLocalTx(const Core::Mat44f& localTx);

		const Core::Guid& GetParentGuid() const;
		const Systems::GameObject* GetParent() const;

		void SetParent(Systems::GameObject* pParentGo);

		const Core::Array<Core::Guid>& GetChildrenGuid() const;

		void AddChild(Systems::GameObject* pChild);
		void RemoveChild(const Core::Guid& child);

		void AddChildCachedPointer(Systems::GameObject* pGo);

		void Update() override;

		void ComputeWorldTx();

	private:
		Core::Sqt m_localSqt;

		Core::Guid m_parent;
		Core::Array<Core::Guid> m_children;

		START_REFLECTION(Systems::TransformComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_localSqt)
			ADD_FIELD_ATTR(m_parent, Core::Hidden)
			ADD_FIELD_ATTR(m_children, Core::Hidden)
		END_REFLECTION()

		// Cached pointer to the parent game object.
		Systems::GameObject* m_pParentGo;

		// Cached pointers to the children game objects.
		Core::Array<Systems::GameObject*> m_childrenGo;

		Core::Mat44f m_worldTx;
	};
}

