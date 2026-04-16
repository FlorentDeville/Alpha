/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Systems/Game/Subsystems/CameraSubsystem.h"

namespace Systems
{
	CameraSubsystem::CameraSubsystem()
		: m_cameraStack()
	{ }

	CameraSubsystem::~CameraSubsystem()
	{ }

	void CameraSubsystem::PushCamera(Rendering::Camera* pCamera)
	{
		m_cameraStack.push(pCamera);
	}

	void CameraSubsystem::PopCamera()
	{
		m_cameraStack.pop();
	}

	const Rendering::Camera* CameraSubsystem::GetTopCamera() const
	{
		return m_cameraStack.top();
	}
}
