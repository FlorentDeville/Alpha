/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Widgets/GizmoModel.h"

#include "Core/Math/Sqt.h"
#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Systems/Objects/GameObject.h"

namespace Editors
{
	GizmoModel::GizmoModel()
		: m_pGo(nullptr)
		, m_onNodeChangedEvent()
		, m_cidOnTransformChanged()
	{ }

	GizmoModel::~GizmoModel()
	{
		if (m_cidOnTransformChanged.IsValid())
			ObjectWatcher::Get().RemoveWatcher(m_cidOnTransformChanged);
	}

	void GizmoModel::SetGameObject(Systems::GameObject* pGo)
	{
		if (m_pGo == pGo)
			return;

		if (m_cidOnTransformChanged.IsValid())
			ObjectWatcher::Get().RemoveWatcher(m_cidOnTransformChanged);

		m_pGo = pGo;

		Core::Guid guid;
		if (pGo)
		{
			guid = pGo->GetGuid();
			m_cidOnTransformChanged = ObjectWatcher::Get().AddWatcher(&pGo->GetTransform(), [this](Systems::Object*, const Systems::FieldDescriptor*, ObjectWatcher::OPERATION, uint32_t)
				{
					//I need to compute manually the world transform here cause the Update didn't run yet.
					m_pGo->GetTransform().ComputeWorldTx();
					m_onNodeChangedEvent(m_pGo->GetGuid());
				});
		}

		m_onNodeChangedEvent(guid);
	}

	bool GizmoModel::ShouldRender()
	{
		return m_pGo != nullptr;
	}

	Core::CallbackId GizmoModel::OnNodeChanged(const OnNodeChangedEvent::Callback& callback)
	{
		return m_onNodeChangedEvent.Connect(callback);
	}

	const Core::Mat44f GizmoModel::GetTransform() const
	{
		if (!m_pGo)
			return Core::Mat44f::s_identity;

		Core::Mat44f txWs = m_pGo->GetTransform().GetWorldTx();
		
		//remove the scale
		for (int ii = 0; ii < 3; ++ii)
		{
			Core::Vec4f axis = txWs.GetRow(ii);
			axis.Normalize();
			txWs.SetRow(ii, axis);
		}

		return txWs;
	}

	void GizmoModel::Translate(const Core::Vec4f& worldPos)
	{
		if (!m_pGo)
			return;
	
		//new world transform
		Core::Mat44f newWorldTx = m_pGo->GetTransform().GetWorldTx();
		newWorldTx.SetRow(3, worldPos);

		Core::Mat44f parentTx = m_pGo->GetTransform().GetParentWorldTx();
		Core::Mat44f invParentTx = parentTx.Inverse();

		Core::Mat44f newLocalTx = newWorldTx * invParentTx;

		m_pGo->GetTransform().SetLocalTx(newLocalTx);

		SendSignalToObjectWatcher();
	}

	void GizmoModel::Rotate(const Core::Mat44f& rotation)
	{
		if (!m_pGo)
			return;

		Core::Mat44f oldTxWs = m_pGo->GetTransform().GetWorldTx();

		Core::Sqt sqtWs(oldTxWs);
		Core::Mat44f baseScale = Core::Mat44f::CreateScaleMatrix(sqtWs.GetScale());

		sqtWs.SetScale(Core::Vec4f(1, 1, 1, 0)); //translation and rotation, remove the scale
		Core::Mat44f baseTransformation = sqtWs.GetMatrix();

		Core::Mat44f txWs = baseScale * rotation * baseTransformation;

		Core::Mat44f txPs = m_pGo->GetTransform().GetParentWorldTx();
		Core::Mat44f invTxPs = txPs.Inverse();			//should be cached

		Core::Mat44f txLs = txWs * invTxPs;
		m_pGo->GetTransform().SetLocalTx(txLs);

		SendSignalToObjectWatcher();
	}

	void GizmoModel::Scale(const Core::Vec4f& scale)
	{
		if (!m_pGo)
			return;

		Core::Mat44f oldTxWs = m_pGo->GetTransform().GetWorldTx();

		Core::Mat44f newScale = Core::Mat44f::CreateScaleMatrix(scale);
		Core::Mat44f newTxWs = newScale * oldTxWs;

		Core::Mat44f txPs = m_pGo->GetTransform().GetParentWorldTx();
		Core::Mat44f invTxPs = txPs.Inverse();			//should be cached

		Core::Mat44f txLs = newTxWs * invTxPs;
		m_pGo->GetTransform().SetLocalTx(txLs);

		SendSignalToObjectWatcher();
	}

	void GizmoModel::SendSignalToObjectWatcher()
	{
		Systems::TransformComponent* pTransform = &m_pGo->GetTransform();
		Systems::FieldDescriptor* pField = pTransform->GetTypeDescriptor()->GetFields()[0];
		ObjectWatcher::Get().SendFieldModifiedEvent(pTransform, pField, ObjectWatcher::SET_FIELD, 0);
	}
}