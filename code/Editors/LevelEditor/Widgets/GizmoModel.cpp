/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Widgets/GizmoModel.h"

#include "Core/Math/Sqt.h"
#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/Component.h"
#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/LevelEditor/LevelMgr.h"
#include "Editors/LevelEditor/SceneTree/Entity.h"
#include "Editors/LevelEditor/SceneTree/Node.h"
#include "Editors/LevelEditor/SceneTree/SceneTree.h"

namespace Editors
{
	GizmoModel::GizmoModel()
		: m_nodeGuid()
		, m_onNodeChangedEvent()
		, m_cidOnPropertyValueChanged()
	{
		m_default.SetIdentity();
	}

	GizmoModel::~GizmoModel()
	{
		if (Entity* pEntity = GetEntity())
		{
			if (Property* pProperty = pEntity->GetProperty("Transform", "Local"))
			{
				pProperty->DisconnectOnValueChanged(m_cidOnPropertyValueChanged);
			}
		}
	}

	void GizmoModel::SetNode(const Os::Guid& nodeGuid)
	{
		if (m_nodeGuid == nodeGuid)
			return;

		if (Entity* pEntity = GetEntity())
		{
			if(Property* pProperty = pEntity->GetProperty("Transform", "Local"))
			{
				pProperty->DisconnectOnValueChanged(m_cidOnPropertyValueChanged);
			}
		}
		
		m_nodeGuid = nodeGuid;

		if (Entity* pEntity = GetEntity())
		{
			if (Property* pProperty = pEntity->GetProperty("Transform", "Local"))
			{
				m_cidOnPropertyValueChanged = pProperty->OnValueChanged([this](const PropertyValue& oldValue, const PropertyValue& newValue) 
					{
						if (m_onNodeChangedEvent)
						{
							m_onNodeChangedEvent(m_nodeGuid);
						}
					});
			}
		}
		
		m_onNodeChangedEvent(nodeGuid);
	}

	bool GizmoModel::ShouldRender()
	{
		return m_nodeGuid.IsValid();
	}

	Core::CallbackId GizmoModel::OnNodeChanged(const OnNodeChangedEvent::Callback& callback)
	{
		return m_onNodeChangedEvent.Connect(callback);
	}

	const Core::Mat44f GizmoModel::GetTransform() const
	{
		const Entity* pEntity = GetConstEntity();
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
		Entity* pEntity = GetEntity();
		if (!pEntity)
			return;

		Core::Mat44f txLs = pEntity->GetLs();
		txLs.SetRow(3, translate);

		pEntity->SetLs(txLs);
	}

	void GizmoModel::Rotate(const Core::Mat44f& rotation)
	{
		Entity* pEntity = GetEntity();
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
		Entity* pEntity = GetEntity();
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

	const Entity* GizmoModel::GetConstEntity() const
	{
		if (!m_nodeGuid.IsValid())
			return nullptr;

		const LevelMgr* pLevelMgr = LevelEditorModule::Get().GetConstLevelMgr();
		if (!pLevelMgr)
			return nullptr;

		const Node* pNode = pLevelMgr->GetConstSceneTree()->GetConstNode(m_nodeGuid);
		if (!pNode)
			return nullptr;

		const Entity* pEntity = pNode->ToConstEntity();
		return pEntity;
	}

	Entity* GizmoModel::GetEntity() const
	{
		if (!m_nodeGuid.IsValid())
			return nullptr;

		LevelMgr* pLevelMgr = LevelEditorModule::Get().GetLevelMgr();
		if (!pLevelMgr)
			return nullptr;

		Node* pNode = pLevelMgr->GetSceneTree()->GetNode(m_nodeGuid);
		if (!pNode)
			return nullptr;

		Entity* pEntity = pNode->ToEntity();
		return pEntity;
	}
}