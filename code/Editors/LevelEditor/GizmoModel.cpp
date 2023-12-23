/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/GizmoModel.h"

#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/Node.h"

namespace Editors
{
	GizmoModel::GizmoModel()
		: m_pNode(nullptr)
		, m_onNodeChangedEvent()
	{
		m_default = DirectX::XMMatrixIdentity();
	}

	GizmoModel::~GizmoModel()
	{}

	void GizmoModel::SetNode(Node* pNode)
	{
		if (m_pNode == pNode)
			return;

		m_pNode = pNode;
		m_onNodeChangedEvent(pNode);
	}

	Core::CallbackId GizmoModel::OnNodeChanged(const OnNodeChangedEvent::Callback& callback)
	{
		return m_onNodeChangedEvent.Connect(callback);
	}

	const DirectX::XMMATRIX GizmoModel::GetTransform() const
	{
		if (!m_pNode)
			return m_default;

		const Entity* pEntity = m_pNode->ToConstEntity();
		if (!pEntity)
			return m_default;

		Core::Mat44f txWs = pEntity->ComputeWs();

		//remove the scale
		for (int ii = 0; ii < 3; ++ii)
		{
			Core::Vec4f axis = txWs.GetRow(ii);
			axis.Normalize();
			txWs.SetRow(ii, axis);
		}

		DirectX::XMMATRIX res = DirectX::XMMatrixSet(
			txWs.Get(0, 0), txWs.Get(0, 1), txWs.Get(0, 2), txWs.Get(0, 3),
			txWs.Get(1, 0), txWs.Get(1, 1), txWs.Get(1, 2), txWs.Get(1, 3),
			txWs.Get(2, 0), txWs.Get(2, 1), txWs.Get(2, 2), txWs.Get(2, 3),
			txWs.Get(3, 0), txWs.Get(3, 1), txWs.Get(3, 2), txWs.Get(3, 3));

		return res;
	}
}