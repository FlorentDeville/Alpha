/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/Gizmo/GizmoModelSqt.h"

//#include "Core/Math/Sqt.h"
#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/ObjectWatcher/ObjectWatcher.h"

//#include "Systems/Objects/GameObject.h"
#include "Systems/Assets/AssetObjects/Mesh/AttachPoint.h"

namespace Editors
{
	GizmoModelSqt::GizmoModelSqt()
		: IGizmoModel()
		, m_pAttachPoint(nullptr)
	{ }

	GizmoModelSqt::~GizmoModelSqt()
	{
		/*if (m_cidOnTransformChanged.IsValid())
			ObjectWatcher::Get().RemoveWatcher(m_cidOnTransformChanged);*/
	}

	void GizmoModelSqt::SetAttachPoint(Systems::AttachPoint* pAttachPoint)
	{
		if (m_pAttachPoint == pAttachPoint)
			return;

		//if (m_cidOnTransformChanged.IsValid())
		//	ObjectWatcher::Get().RemoveWatcher(m_cidOnTransformChanged);

		m_pAttachPoint = pAttachPoint;

		/*Core::Guid guid;
		if (pGo)
		{
			guid = pGo->GetGuid();
			m_cidOnTransformChanged = ObjectWatcher::Get().AddWatcher(&pGo->GetTransform(), [this](void*, const Core::FieldDescriptor*, ObjectWatcher::OPERATION, uint32_t)
				{
					m_onTargetChanged();
				});
		}*/

		m_onTargetChanged();
	}

	bool GizmoModelSqt::ShouldRender()
	{
		return m_pAttachPoint != nullptr;
	}

	const Core::Mat44f GizmoModelSqt::GetTransform() const
	{
		if (!m_pAttachPoint)
			return Core::Mat44f::s_identity;

		Core::Mat44f txWs = m_pAttachPoint->GetLocator().GetMatrix();

		//remove the scale
		for (int ii = 0; ii < 3; ++ii)
		{
			Core::Vec4f axis = txWs.GetRow(ii);
			axis.Normalize();
			txWs.SetRow(ii, axis);
		}

		return txWs;
	}

	void GizmoModelSqt::Translate(const Core::Vec4f& worldPos)
	{
		if (!m_pAttachPoint)
			return;

		m_pAttachPoint->GetLocator().SetTranslation(worldPos);
		//new world transform
		/*Core::Mat44f newWorldTx = m_pSqt->SetTranslation(worldPos)
		newWorldTx.SetRow(3, worldPos);

		Core::Mat44f parentTx = m_pGo->GetTransform().GetParentWorldTx();
		Core::Mat44f invParentTx = parentTx.Inverse();

		Core::Mat44f newLocalTx = newWorldTx * invParentTx;

		m_pGo->GetTransform().SetLocalTx(newLocalTx);

		SendSignalToObjectWatcher();*/
	}

	void GizmoModelSqt::Rotate(const Core::Mat44f& rotation)
	{
		if (!m_pAttachPoint)
			return;

		Core::Mat44f wsTx = rotation * m_pAttachPoint->GetLocator().GetMatrix();
		m_pAttachPoint->GetLocator() = Core::Sqt(wsTx);
		//Core::Mat44f oldTxWs = m_pGo->GetTransform().GetWorldTx();

		//Core::Sqt sqtWs(oldTxWs);
		//Core::Mat44f baseScale = Core::Mat44f::CreateScaleMatrix(sqtWs.GetScale());

		//sqtWs.SetScale(Core::Vec4f(1, 1, 1, 0)); //translation and rotation, remove the scale
		//Core::Mat44f baseTransformation = sqtWs.GetMatrix();

		//Core::Mat44f txWs = baseScale * rotation * baseTransformation;

		//Core::Mat44f txPs = m_pGo->GetTransform().GetParentWorldTx();
		//Core::Mat44f invTxPs = txPs.Inverse();			//should be cached

		//Core::Mat44f txLs = txWs * invTxPs;
		//m_pGo->GetTransform().SetLocalTx(txLs);

		//SendSignalToObjectWatcher();
	}

	void GizmoModelSqt::IncrementScale(const Core::Vec4f& scale)
	{
		if (!m_pAttachPoint)
			return;

		//const Core::Mat44f localTx = m_pSqt->GetScale()
		//float scaleX = localTx.GetX().Length();
		//float scaleY = localTx.GetY().Length();
		//float scaleZ = localTx.GetZ().Length();

		Core::Vec4f localScale = m_pAttachPoint->GetLocator().GetScale();
		Core::Vec4f newLocalScale = localScale + scale;

		Core::Vec4f oneOverScale(1.f / localScale.GetX(), 1.f / localScale.GetY(), 1.f / localScale.GetZ(), 0);
		Core::Vec4f newScale = m_pAttachPoint->GetLocator().GetTranslation() * oneOverScale * newLocalScale;

		m_pAttachPoint->GetLocator().SetScale(newScale);
		/*Core::Mat44f newLocalTx;
		newLocalTx.SetRow(0, localTx.GetX() * (1.f / scaleX * newLocalScale.GetX()));
		newLocalTx.SetRow(1, localTx.GetY() * (1.f / scaleY * newLocalScale.GetY()));
		newLocalTx.SetRow(2, localTx.GetZ() * (1.f / scaleZ * newLocalScale.GetZ()));
		newLocalTx.SetRow(3, localTx.GetT());

		m_pGo->GetTransform().SetLocalTx(newLocalTx);

		SendSignalToObjectWatcher();*/
	}

	//void GizmoModelSqt::SendSignalToObjectWatcher()
	//{
	//	Systems::TransformComponent* pTransform = &m_pGo->GetTransform();
	//	Core::FieldDescriptor* pField = pTransform->GetTypeDescriptor()->GetFields()[0];
	//	ObjectWatcher::Get().SendFieldModifiedEvent(pTransform, pField, ObjectWatcher::SET_FIELD, 0);
	//}
}
