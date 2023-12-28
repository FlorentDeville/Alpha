/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackList.h"
#include "Core/Math/Mat44f.h"

namespace Core
{
	class Mat44f;
}

namespace Editors
{
	class Node;

	class GizmoModel
	{
	public:
		GizmoModel();
		~GizmoModel();

		void SetNode(Node* pNode);

		bool ShouldRender();

		using OnNodeChangedEvent = Core::CallbackList<void(Node*)>;

		Core::CallbackId OnNodeChanged(const OnNodeChangedEvent::Callback& callback);

		//Function called by the gizmo to be placed in the correct location/orientation. Typically doesn't contain the scale.
		virtual const Core::Mat44f GetTransform() const;

		//The gizmo sends the new world space position
		virtual void Translate(const Core::Vec4f& translate);

		//The gizmo sends the new rotation to apply
		virtual void Rotate(const Core::Mat44f& rotation);

		//The gizmo sends a new scale to apply
		virtual void Scale(const Core::Vec4f& scale);

	private:
		Node* m_pNode;

		OnNodeChangedEvent m_onNodeChangedEvent;

		Core::Mat44f m_default;
	};
}
