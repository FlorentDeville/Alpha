/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/Widgets/Gizmo/IGizmoModel.h"

namespace Core
{
	class Sqt;
	class Mat44f;
	class Vec4f;
}

namespace Editors
{
	class GizmoModelSqt : public IGizmoModel
	{
	public:
		GizmoModelSqt();
		~GizmoModelSqt();

		void SetSqt(Core::Sqt* pSqt);

		//Base function 
		bool ShouldRender() override;
		const Core::Mat44f GetTransform() const override;
		void Translate(const Core::Vec4f& worldPos) override;
		virtual void Rotate(const Core::Mat44f& rotation) override;
		virtual void IncrementScale(const Core::Vec4f& scale) override;

	private:
		Core::Sqt* m_pSqt;
	};
}
