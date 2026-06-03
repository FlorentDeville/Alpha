/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/ObjectWatcher/ObjectWatcherCallbackId.h"
#include "Editors/Widgets/Gizmo/IGizmoModel.h"

namespace Core
{
	class Mat44f;
	class Vec4f;
}

namespace Systems
{
	class MeshAsset;
}

namespace Editors
{
	class GizmoModelAttachPoint : public IGizmoModel
	{
	public:
		GizmoModelAttachPoint();
		~GizmoModelAttachPoint();

		void SetAttachPoint(Systems::MeshAsset* pMesh, uint32_t index);

		//Base function 
		bool ShouldRender() override;
		const Core::Mat44f GetTransform() const override;
		void Translate(const Core::Vec4f& worldPos) override;
		virtual void Rotate(const Core::Mat44f& rotation) override;
		virtual void IncrementScale(const Core::Vec4f& scale) override;

	private:
		Systems::MeshAsset* m_pMesh;
		uint32_t m_attachPointIndex;

		ObjectWatcherCallbackId m_callbackId;

		void SendSignalToObjectWatcher();
		bool IsValid() const;
	};
}
