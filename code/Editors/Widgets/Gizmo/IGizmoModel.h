/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Callbacks/CallbackMacro.h"

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
	class IGizmoModel
	{
	public:
		IGizmoModel() = default;
		~IGizmoModel() = default;

		virtual bool ShouldRender() = 0;

		//Function called by the gizmo to be placed in the correct location/orientation. Typically doesn't contain the scale.
		virtual const Core::Mat44f GetTransform() const = 0;

		//The gizmo sends the new world position
		virtual void Translate(const Core::Vec4f& worldPos) = 0;

		//The gizmo sends the new rotation to apply
		virtual void Rotate(const Core::Mat44f& rotation) = 0;

		//Increment the local transform scale by the value passed in parameter
		virtual void IncrementScale(const Core::Vec4f& scale) = 0;

		//Event to call when the game object tracked by the gizmo has changed.
		EVENT_DECL(TargetChanged, void())
	};
}
