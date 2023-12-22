/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackList.h"

#include <DirectXMath.h>

namespace Editors
{
	class Node;

	class GizmoModel
	{
	public:
		GizmoModel();
		~GizmoModel();

		void SetNode(Node* pNode);

		
		using OnNodeChangedEvent = Core::CallbackList<void(Node*)>;

		Core::CallbackId OnNodeChanged(const OnNodeChangedEvent::Callback& callback);

		//Function called by the gizmo to be placed in the correct location/orientation
		virtual const DirectX::XMMATRIX GetTransform() const;

	private:
		Node* m_pNode;

		OnNodeChangedEvent m_onNodeChangedEvent;

		DirectX::XMMATRIX m_default;
	};
}
