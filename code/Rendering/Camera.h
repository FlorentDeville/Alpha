/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

namespace Rendering
{
	class Camera
	{
	public:
		Camera();
		~Camera();

		void SetLookAt(const DirectX::XMVECTOR& position, const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& up);
		void SetProjection(float fovInRadian, float aspectRatio, float nearDistance, float farDistance);

		float GetFOV() const;
		float GetAspectRatio() const;
		float GetNearDistance() const;
		float GetFarDistance() const;

		const DirectX::XMMATRIX& GetViewMatrix() const;
		const DirectX::XMMATRIX& GetProjectionMatrix() const;

	private:
		DirectX::XMMATRIX m_view;
		DirectX::XMMATRIX m_projection;

		//projection 
		float m_fov;
		float m_aspectRatio;
		float m_nearDistance;
		float m_farDistance;
	};
}
