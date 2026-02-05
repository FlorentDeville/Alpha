/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackList.h"

#include "Editors/ObjectWatcher/ObjectWatcherCallbackId.h"

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
	class GizmoModel
	{
	public:
		GizmoModel();
		~GizmoModel();

		void SetGameObject(Systems::GameObject* pGo);

		bool ShouldRender();

		using OnNodeChangedEvent = Core::CallbackList<void(const Core::Guid& nodeGuid)>;

		Core::CallbackId OnNodeChanged(const OnNodeChangedEvent::Callback& callback);

		//Function called by the gizmo to be placed in the correct location/orientation. Typically doesn't contain the scale.
		virtual const Core::Mat44f GetTransform() const;

		//The gizmo sends the new translation to apply
		virtual void Translate(const Core::Vec4f& translate);

		//The gizmo sends the new rotation to apply
		virtual void Rotate(const Core::Mat44f& rotation);

		//The gizmo sends a new scale to apply
		virtual void Scale(const Core::Vec4f& scale);

	private:
		Systems::GameObject* m_pGo;

		//Event called when the game object tracked by the gizmo has changed. Typically when the selected game object has changed.
		OnNodeChangedEvent m_onNodeChangedEvent;

		//Callback attached to the object watcher transform
		ObjectWatcherCallbackId m_cidOnTransformChanged;

		//When the gizmo modifies the transform, send a signal to the object watcher so other widgets can refresh.
		void SendSignalToObjectWatcher();
	};
}
