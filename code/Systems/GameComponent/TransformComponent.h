/********************************************************************/
/* � 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Systems/Objects/GameComponent.h"
#include "Systems/Reflection/ReflectionCoreTypes.h"

namespace Systems
{
	ENABLE_REFLECTION_WITH_NS(Systems, TransformComponent)
	class TransformComponent : public GameComponent
	{
	public:
		TransformComponent();
		~TransformComponent();

		const Core::Mat44f& GetLocalTx() const;
		void SetLocalTx(const Core::Mat44f& localTx);

		const Core::Guid& GetParentGuid() const;
		void SetParentGuid(const Core::Guid& parentGuid);

		const Core::Array<Core::Guid>& GetChildrenGuid() const;

		void AddChild(const Core::Guid& child);
		void RemoveChild(const Core::Guid& child);

	private:
		Core::Mat44f m_localTransform;

		Core::Guid m_parent;
		Core::Array<Core::Guid> m_children;

		START_REFLECTION(Systems::TransformComponent)
			ADD_BASETYPE(Systems::GameComponent)
			ADD_FIELD(m_localTransform)
			ADD_FIELD_ATTR(m_parent, Hidden)
			ADD_FIELD_ATTR(m_children, Hidden)
		END_REFLECTION()
	};
}

