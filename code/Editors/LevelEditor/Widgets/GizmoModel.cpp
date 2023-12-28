/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Widgets/GizmoModel.h"

#include "Core/Math/Sqt.h"
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

	bool GizmoModel::ShouldRender()
	{
		return m_pNode != nullptr;
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

	void GizmoModel::Translate(const Core::Vec4f& translate)
	{
		if (!m_pNode)
			return;

		Entity* pEntity = m_pNode->ToEntity();
		if (!pEntity)
			return;

		Core::Mat44f txLs = pEntity->GetLs();
		txLs.SetRow(3, translate);

		pEntity->SetLs(txLs);
	}

	void GizmoModel::Rotate(const Core::Mat44f& rotation)
	{
		if (!m_pNode)
			return;

		Entity* pEntity = m_pNode->ToEntity();
		if (!pEntity)
			return;

		Core::Mat44f oldTxWs = pEntity->ComputeWs();

		Core::Sqt sqtWs(oldTxWs);
		Core::Mat44f baseScale = Core::Mat44f::CreateScaleMatrix(sqtWs.GetScale());

		sqtWs.SetScale(Core::Vec4f(1, 1, 1, 0)); //translation and rotation, remove the scale
		Core::Mat44f baseTransformation = sqtWs.GetMatrix();

		Core::Mat44f txWs = baseScale * rotation * baseTransformation;

		Core::Mat44f txPs = pEntity->ComputeParentWs(); //should be cached
		Core::Mat44f invTxPs = txPs.Inverse();			//should be cached

		Core::Mat44f txLs = txWs * invTxPs;
		pEntity->SetLs(txLs);
	}

	void GizmoModel::Scale(const Core::Vec4f& scale)
	{
		if (!m_pNode)
			return;

		Entity* pEntity = m_pNode->ToEntity();
		if (!pEntity)
			return;

		Core::Mat44f oldTxWs = pEntity->ComputeWs();

		Core::Mat44f newScale = Core::Mat44f::CreateScaleMatrix(scale);
		Core::Mat44f newTxWs = newScale * oldTxWs;

		Core::Mat44f txPs = pEntity->ComputeParentWs(); //should be cached
		Core::Mat44f invTxPs = txPs.Inverse();			//should be cached

		Core::Mat44f txLs = newTxWs * invTxPs;
		pEntity->SetLs(txLs);
	}
}