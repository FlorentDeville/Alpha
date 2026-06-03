/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/Gizmo/GizmoModelAttachPoint.h"

#include "Core/Math/Vec4f.h"

#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Assets/AssetObjects/Mesh/AttachPoint.h"
#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"

namespace Editors
{
	GizmoModelAttachPoint::GizmoModelAttachPoint()
		: IGizmoModel()
		, m_pMesh(nullptr)
		, m_attachPointIndex(UINT32_MAX)
		, m_callbackId()
	{ }

	GizmoModelAttachPoint::~GizmoModelAttachPoint()
	{
		if (m_callbackId.IsValid())
			ObjectWatcher::Get().RemoveWatcher(m_callbackId);
	}

	void GizmoModelAttachPoint::SetAttachPoint(Systems::MeshAsset* pMesh, uint32_t index)
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

	bool GizmoModelAttachPoint::ShouldRender()
	{
		return m_pMesh != nullptr && m_pMesh->GetAttachPoints().IsValidIndex(m_attachPointIndex);
	}

	const Core::Mat44f GizmoModelAttachPoint::GetTransform() const
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

	void GizmoModelAttachPoint::Translate(const Core::Vec4f& worldPos)
	{
		if (!IsValid())
			return;

		Systems::AttachPoint& ap = m_pMesh->GetAttachPoints()[m_attachPointIndex];
		ap.GetLocator().SetTranslation(worldPos);

		SendSignalToObjectWatcher();
	}

	void GizmoModelAttachPoint::Rotate(const Core::Mat44f& rotation)
	{
		if (!IsValid())
			return;

		Systems::AttachPoint& ap = m_pMesh->GetAttachPoints()[m_attachPointIndex];
		Core::Mat44f wsTx = rotation * ap.GetLocator().GetMatrix();
		ap.GetLocator() = Core::Sqt(wsTx);

		SendSignalToObjectWatcher();
	}

	void GizmoModelAttachPoint::IncrementScale(const Core::Vec4f& scale)
	{
		if (!IsValid())
			return;

		Systems::AttachPoint& ap = m_pMesh->GetAttachPoints()[m_attachPointIndex];
		Core::Vec4f localScale = ap.GetLocator().GetScale();
		Core::Vec4f newLocalScale = localScale + scale;

		Core::Vec4f oneOverScale(1.f / localScale.GetX(), 1.f / localScale.GetY(), 1.f / localScale.GetZ(), 0);
		Core::Vec4f newScale = ap.GetLocator().GetTranslation() * oneOverScale * newLocalScale;

		ap.GetLocator().SetScale(newScale);

		SendSignalToObjectWatcher();
	}

	void GizmoModelAttachPoint::SendSignalToObjectWatcher()
	{
		const Core::TypeDescriptor* pType = Core::TypeResolver<Systems::AttachPoint>::GetConstType();
		const Core::FieldDescriptor* pField = pType->FindField("m_locator");

		ObjectWatcher::Get().SendFieldModifiedEvent(&m_pMesh->GetAttachPoints()[m_attachPointIndex], pField, ObjectWatcher::SET_FIELD, 0);
	}

	bool GizmoModelAttachPoint::IsValid() const
	{
		return m_pMesh != nullptr && m_pMesh->GetAttachPoints().IsValidIndex(m_attachPointIndex);
	}
}
