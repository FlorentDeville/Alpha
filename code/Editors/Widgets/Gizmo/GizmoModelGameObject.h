/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/ObjectWatcher/ObjectWatcherCallbackId.h"
#include "Editors/Widgets/Gizmo/IGizmoModel.h"

namespace Core
{
	class Guid;
	class Mat44f;
	class Vec4f;
}

namespace Systems
{
	class GameObject;
}

namespace Editors
{
	class GizmoModelGameObject : public IGizmoModel
	{
	public:
		GizmoModelGameObject();
		~GizmoModelGameObject();

		void SetGameObject(Systems::GameObject* pGo);

		//Base function 
		bool ShouldRender() override;
		const Core::Mat44f GetTransform() const override;
		void Translate(const Core::Vec4f& worldPos) override;
		virtual void Rotate(const Core::Mat44f& rotation) override;
		virtual void IncrementScale(const Core::Vec4f& scale) override;

	private:
		Systems::GameObject* m_pGo;

		//Callback attached to the object watcher transform
		ObjectWatcherCallbackId m_cidOnTransformChanged;

		//When the gizmo modifies the transform, send a signal to the object watcher so other widgets can refresh.
		void SendSignalToObjectWatcher();
	};
}
