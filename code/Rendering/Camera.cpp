/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/Camera.h"

namespace Rendering
{
	Camera::Camera()
		: m_view()
		, m_projection()
		, m_fov(DirectX::XMConvertToRadians(45.f))
		, m_aspectRatio(16.f/9.f)
		, m_nearDistance(0.1f)
		, m_farDistance(1000.f)
		, m_position(0, 0, 0, 1)
	{}

	Camera::~Camera()
	{}

	void Camera::SetLookAt(const Core::Vec4f& position, const Core::Vec4f& target, const Core::Vec4f& up)
	{
		m_position = position;
		m_position.Set(3, 1);

		Core::Vec4f cameraDirection = target - position;
		cameraDirection.Normalize();
		
		Core::Mat44f view = Core::Mat44f::CreateView(position, cameraDirection, up);
		m_view = view.m_matrix;
	}

	void Camera::SetProjection(float fovInRadian, float aspectRatio, float nearDistance, float farDistance)
	{
		m_fov = fovInRadian;
		m_aspectRatio = aspectRatio;
		m_nearDistance = nearDistance;
		m_farDistance = farDistance;

		DirectX::XMMATRIX m = DirectX::XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearDistance, m_farDistance);
		m_projection = Core::Mat44f::CreatePerspective(m_fov, m_aspectRatio, m_nearDistance, m_farDistance);
	}

	float Camera::GetFOV() const
	{
		return m_fov;
	}

	float Camera::GetAspectRatio() const
	{
		return m_aspectRatio;
	}

	float Camera::GetNearDistance() const
	{
		return m_nearDistance;
	}

	float Camera::GetFarDistance() const
	{
		return m_farDistance;
	}

	const DirectX::XMMATRIX& Camera::GetViewMatrix() const
	{
		return m_view;
	}

	const Core::Mat44f& Camera::GetProjectionMatrix() const
	{
		return m_projection;
	}

	const Core::Vec4f& Camera::GetPosition() const
	{
		return m_position;
	}
}
