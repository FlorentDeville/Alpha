/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

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
	{}

	Camera::~Camera()
	{}

	void Camera::SetLookAt(const DirectX::XMVECTOR& position, const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& up)
	{
		DirectX::XMVECTOR cameraDirection = DirectX::XMVectorSubtract(target, position);
		cameraDirection = DirectX::XMVector4Normalize(cameraDirection);

		m_view = DirectX::XMMatrixLookToLH(position, cameraDirection, up);
	}

	void Camera::SetProjection(float fovInRadian, float aspectRatio, float nearDistance, float farDistance)
	{
		m_fov = fovInRadian;
		m_aspectRatio = aspectRatio;
		m_nearDistance = nearDistance;
		m_farDistance = farDistance;

		m_projection = DirectX::XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearDistance, m_farDistance);
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

	const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const
	{
		return m_projection;
	}
}
