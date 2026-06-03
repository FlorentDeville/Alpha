/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

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
	class GizmoModelSqt : public IGizmoModel
	{
	public:
		GizmoModelSqt();
		~GizmoModelSqt();

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

		bool IsValid() const;
	};
}
