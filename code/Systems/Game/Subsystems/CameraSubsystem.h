/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <stack>

namespace Rendering
{
	class Camera;
}

namespace Systems
{
	class CameraSubsystem
	{
	public:
		CameraSubsystem();
		~CameraSubsystem();

		void PushCamera(Rendering::Camera* pCamera);
		void PopCamera();

		const Rendering::Camera* GetTopCamera() const;

	private:
		//Stack of camera. The currently active camera is the one at the top. There is always a default camera.
		std::stack<Rendering::Camera*> m_cameraStack;
	};
}
