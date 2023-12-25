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
		m_default.SetIdentity();
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

	const Core::Mat44f GizmoModel::GetTransform() const
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

		return txWs;
	}

	void GizmoModel::Translate(const Core::Mat44f& txWs)
	{
		if (!m_pNode)
			return;

		Entity* pEntity = m_pNode->ToEntity();
		if (!pEntity)
			return;

		Core::Mat44f txPs = pEntity->ComputeParentWs(); //should be cached
		Core::Mat44f invTxPs = txPs.Inverse();			//should be cached
		Core::Mat44f txLs = txWs * invTxPs;
		pEntity->SetLs(txLs);
	}
}