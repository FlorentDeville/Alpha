/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Entities/CameraEntity.h"

CameraEntity::CameraEntity(float aspectRatio)
	: LocatorEntity()
	, m_near(0.1f)
	, m_far(100.f)
	, m_fov(45)
	, m_aspectRatio(aspectRatio)
{
	m_position = DirectX::XMVectorSet(0, 10, -10, 1);
	m_direction = DirectX::XMVectorSubtract(DirectX::XMVectorSet(0, 0, 0, 1), m_position);
	m_up = DirectX::XMVectorSet(0, 1, 0, 0);

	float fovRad = DirectX::XMConvertToRadians(m_fov);
	m_projection = DirectX::XMMatrixPerspectiveFovLH(fovRad, aspectRatio, m_near, m_far);
}

CameraEntity::~CameraEntity()
{}

void CameraEntity::Update()
{
	//to inverse, transpose the rotation and negate the translation
	m_view = DirectX::XMMatrixLookToLH(m_position, m_direction, m_up);
	
	DirectX::XMVECTOR translation = m_view.r[3];

	DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(m_view);
	transform.r[3] = DirectX::XMVectorNegate(translation);
	SetTransform(transform);
}

void CameraEntity::Render()
{}

const DirectX::XMMATRIX& CameraEntity::GetView() const
{
	return m_view;
}

const DirectX::XMMATRIX& CameraEntity::GetProjection() const
{
	return m_projection;
}
