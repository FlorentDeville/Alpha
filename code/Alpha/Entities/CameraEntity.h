/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Entities/LocatorEntity.h"

#include "DirectXMath.h"

//Base camera with perspective rendering
class CameraEntity : public LocatorEntity
{
public:
	CameraEntity(float aspectRatio);
	virtual ~CameraEntity();

	void Update() override;
	void Render() override;

	const DirectX::XMMATRIX& GetView() const;
	const DirectX::XMMATRIX& GetProjection() const;

protected:
	DirectX::XMMATRIX m_view;
	DirectX::XMMATRIX m_projection;

	DirectX::XMVECTOR m_position;
	DirectX::XMVECTOR m_up;
	DirectX::XMVECTOR m_direction;

	float m_near;
	float m_far;
	float m_fov;
	float m_aspectRatio;
};
