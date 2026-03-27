/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Callbacks/CallbackList.h"
#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include <DirectXMath.h>

namespace Editors
{
	class CameraWidget
	{
	public:
		CameraWidget();
		~CameraWidget();

		void Update(float dtInSeconds);
		void Render(float aspectRatio);

		void SetTransform(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& eulerAngle);
		void SetFov(float fov);

		const Core::Mat44f& GetView() const;
		const Core::Mat44f& GetProjection() const;
		const Core::Vec4f& GetPosition() const;
		float GetFov() const;

		//event
		using OnWsChangedEvent = Core::CallbackList<void(const Core::Mat44f&)>;
		Core::CallbackId OnWsChanged(const OnWsChangedEvent::Callback& callback);

	private:
		enum CameraState
		{
			None,		//nothing
			FPS			//right click + wasd
		};

		//camera transform
		Core::Mat44f m_cameraRotation;			//matrix for the orientation of the camera (same as m_cameraEulerAngle)
		Core::Vec4f m_cameraEulerAngle;			//euler angle of the camera (same as m_cameraRotation)
		Core::Vec4f m_cameraPosition;			//position of the camera in world space
		Core::Mat44f m_cameraTransform;			//transform of the camera in world space

		Core::Mat44f m_view;
		Core::Mat44f m_proj;
		
		float m_fov;

		float m_translationSpeed;
		float m_rotationSpeed;

		bool m_firstFrameMouseDown;

		DirectX::XMINT2 m_mousePreviousPos;

		CameraState m_cameraState;

		OnWsChangedEvent m_onWsChanged;

		void UpdateCamera_None(float dtInSeconds);
		void UpdateCamera_FPS(float dtInSeconds);
	};
}
