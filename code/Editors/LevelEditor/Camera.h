/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

namespace Editors
{
	class Camera
	{
	public:
		Camera();
		~Camera();

		void Update(float dtInSeconds);
		void Render(float aspectRatio);

	private:
		enum CameraState
		{
			None,		//nothing
			FPS			//right click + wasd
		};

		//camera transform
		DirectX::XMMATRIX m_cameraRotation;		//matrix for the orientation of the camera (same as m_cameraEulerAngle)
		DirectX::XMVECTOR m_cameraEulerAngle;	//euler angle of the camera (same as m_cameraRotation)
		DirectX::XMVECTOR m_cameraPosition;		//position of the camera in world space
		DirectX::XMMATRIX m_cameraTransform;	//transform of the camera in world space

		float m_translationSpeed;
		float m_rotationSpeed;

		bool m_firstFrameMouseDown;

		DirectX::XMINT2 m_mousePreviousPos;

		CameraState m_cameraState;

		void UpdateCamera_None(float dtInSeconds);
		void UpdateCamera_FPS(float dtInSeconds);
	};
}
