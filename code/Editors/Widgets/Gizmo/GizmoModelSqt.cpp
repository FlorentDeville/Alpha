/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/Gizmo/GizmoModelSqt.h"

//#include "Core/Math/Sqt.h"
#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Assets/AssetObjects/Mesh/AttachPoint.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"

namespace Editors
{
	GizmoModelSqt::GizmoModelSqt()
		: IGizmoModel()
		, m_pMesh(nullptr)
		, m_attachPointIndex(UINT32_MAX)
		, m_callbackId()
	{ }

	GizmoModelSqt::~GizmoModelSqt()
	{
		if (m_callbackId.IsValid())
			ObjectWatcher::Get().RemoveWatcher(m_callbackId);
	}

	void GizmoModelSqt::SetAttachPoint(Systems::MeshAsset* pMesh, uint32_t index)
	{
		if (pMesh == m_pMesh && m_attachPointIndex == index)
			return;

		if (m_callbackId.IsValid())
			ObjectWatcher::Get().RemoveWatcher(m_callbackId);

		m_pMesh = pMesh;
		m_attachPointIndex = index;

		if (IsValid())
		{
			Systems::AttachPoint& ap = m_pMesh->GetAttachPoints()[m_attachPointIndex];
			m_callbackId = ObjectWatcher::Get().AddWatcher(&ap, [this](void*, const Core::FieldDescriptor*, ObjectWatcher::OPERATION, uint32_t)
				{
					m_onTargetChanged();
				});
		}

		m_onTargetChanged();
	}

	bool GizmoModelSqt::ShouldRender()
	{
		return m_pMesh != nullptr && m_pMesh->GetAttachPoints().IsValidIndex(m_attachPointIndex);
	}

	const Core::Mat44f GizmoModelSqt::GetTransform() const
	{
		if (!IsValid())
			return Core::Mat44f::s_identity;

		Systems::AttachPoint& ap = m_pMesh->GetAttachPoints()[m_attachPointIndex];
		Core::Mat44f txWs = ap.GetLocator().GetMatrix();

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
		if (!IsValid())
			return;

		Systems::AttachPoint& ap = m_pMesh->GetAttachPoints()[m_attachPointIndex];
		ap.GetLocator().SetTranslation(worldPos);
		//new world transform
		/*Core::Mat44f newWorldTx = m_pSqt->SetTranslation(worldPos)
		newWorldTx.SetRow(3, worldPos);

		Core::Mat44f parentTx = m_pGo->GetTransform().GetParentWorldTx();
		Core::Mat44f invParentTx = parentTx.Inverse();

		Core::Mat44f newLocalTx = newWorldTx * invParentTx;

		m_pGo->GetTransform().SetLocalTx(newLocalTx);

		SendSignalToObjectWatcher();*/
		SendSignalToObjectWatcher();
	}

	void GizmoModelSqt::Rotate(const Core::Mat44f& rotation)
	{
		if (!IsValid())
			return;

		Systems::AttachPoint& ap = m_pMesh->GetAttachPoints()[m_attachPointIndex];
		Core::Mat44f wsTx = rotation * ap.GetLocator().GetMatrix();
		ap.GetLocator() = Core::Sqt(wsTx);
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

		SendSignalToObjectWatcher();
	}

	void GizmoModelSqt::IncrementScale(const Core::Vec4f& scale)
	{
		if (!IsValid())
			return;

		//const Core::Mat44f localTx = m_pSqt->GetScale()
		//float scaleX = localTx.GetX().Length();
		//float scaleY = localTx.GetY().Length();
		//float scaleZ = localTx.GetZ().Length();

		Systems::AttachPoint& ap = m_pMesh->GetAttachPoints()[m_attachPointIndex];
		Core::Vec4f localScale = ap.GetLocator().GetScale();
		Core::Vec4f newLocalScale = localScale + scale;

		Core::Vec4f oneOverScale(1.f / localScale.GetX(), 1.f / localScale.GetY(), 1.f / localScale.GetZ(), 0);
		Core::Vec4f newScale = ap.GetLocator().GetTranslation() * oneOverScale * newLocalScale;

		ap.GetLocator().SetScale(newScale);
		/*Core::Mat44f newLocalTx;
		newLocalTx.SetRow(0, localTx.GetX() * (1.f / scaleX * newLocalScale.GetX()));
		newLocalTx.SetRow(1, localTx.GetY() * (1.f / scaleY * newLocalScale.GetY()));
		newLocalTx.SetRow(2, localTx.GetZ() * (1.f / scaleZ * newLocalScale.GetZ()));
		newLocalTx.SetRow(3, localTx.GetT());

		m_pGo->GetTransform().SetLocalTx(newLocalTx);

		SendSignalToObjectWatcher();*/
		SendSignalToObjectWatcher();
	}

	void GizmoModelSqt::SendSignalToObjectWatcher()
	{
		//Systems::TransformComponent* pTransform = &m_pGo->GetTransform();
		const Core::TypeDescriptor* pType = Core::TypeResolver<Systems::AttachPoint>::GetConstType();
		const Core::FieldDescriptor* pField = pType->FindField("m_locator");

		//Core::FieldDescriptor* pField = pTransform->GetTypeDescriptor()->GetFields()[0];
		ObjectWatcher::Get().SendFieldModifiedEvent(&m_pMesh->GetAttachPoints()[m_attachPointIndex], pField, ObjectWatcher::SET_FIELD, 0);
	}

	bool GizmoModelSqt::IsValid() const
	{
		return m_pMesh != nullptr && m_pMesh->GetAttachPoints().IsValidIndex(m_attachPointIndex);
	}
}
