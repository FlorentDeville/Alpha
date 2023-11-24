/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Camera.h"

#include "Inputs/InputMgr.h"

#include "Rendering/Camera.h"
#include "Rendering/RenderModule.h"

//needed for the operators *-+ between vectors and floats.
using namespace DirectX;

namespace Editors
{
	Camera::Camera()
		: m_firstFrameMouseDown(true)
		, m_mousePreviousPos()
		, m_cameraState(CameraState::None)
		, m_translationSpeed(10.f)
		, m_rotationSpeed(0.3f)
	{
		m_cameraPosition = DirectX::XMVectorSet(0, 10, -10, 1);
		m_cameraEulerAngle = DirectX::XMVectorSet(3.14f / 4.f, 0, 0, 0);

		DirectX::XMMATRIX YRotation = DirectX::XMMatrixRotationY(m_cameraEulerAngle.m128_f32[1]);
		DirectX::XMMATRIX XRotation = DirectX::XMMatrixRotationX(m_cameraEulerAngle.m128_f32[0]);
		m_cameraRotation = XRotation * YRotation;

		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(m_cameraPosition);
		m_cameraTransform = m_cameraRotation * translationMatrix;
	}

	Camera::~Camera()
	{}

	void Camera::Update(float dtInSeconds)
	{
		switch (m_cameraState)
		{
		case CameraState::None:
			UpdateCamera_None(dtInSeconds);
			break;

		case CameraState::FPS:
			UpdateCamera_FPS(dtInSeconds);
			break;

		default:
			assert(false);
			break;
		}
	}

	void Camera::Render(float aspectRatio)
	{
		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		Rendering::Camera* pCamera = renderModule.GetCamera();

		DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 1);
		DirectX::XMVECTOR targetOffset = DirectX::XMVectorSet(0, 0, 1, 1);
		DirectX::XMVECTOR cameraLookAt = DirectX::XMVector4Transform(targetOffset, m_cameraTransform);
		DirectX::XMVECTOR cameraPosition = m_cameraTransform.r[3];
		pCamera->SetLookAt(cameraPosition, cameraLookAt, cameraUp);

		constexpr float fovRad = DirectX::XMConvertToRadians(45.f);
		const float nearDistance = 0.1f;
		const float farDistance = 1000.f;
		pCamera->SetProjection(fovRad, aspectRatio, nearDistance, farDistance);
	}

	void Camera::UpdateCamera_None(float dtInSeconds)
	{
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseRightButtonDown())
			m_cameraState = CameraState::FPS;
	}

	void Camera::UpdateCamera_FPS(float dtInSeconds)
	{
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (!inputs.IsMouseRightButtonDown())
		{
			m_firstFrameMouseDown = true;
			m_cameraState = CameraState::None;
			return;
		}

		//speed up/down
		int16_t mouseWheelDistance = inputs.GetMouseWheelDistance();
		if (mouseWheelDistance != 0)
		{
			m_translationSpeed += mouseWheelDistance * 0.05f;
			//m_rotationSpeed += mouseWheelDistance * 0.0001f;

			if (m_translationSpeed < 1) m_translationSpeed = 1.f;
			//if (m_rotationSpeed < 0.003f) m_rotationSpeed = 0.003f;
		}

		bool updateCameraTransform = false;

		if (inputs.IsKeyPressed('W')) //forward
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[2];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('S')) //backward
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[2];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, -zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('A')) //left
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[0];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, -zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
			printf("key D pressed");
		}
		if (inputs.IsKeyPressed('D')) //right
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[0];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
			printf("key D pressed");
		}
		if (inputs.IsKeyPressed('Q')) //up
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[1];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('E')) //down
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[1];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, -zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}

		//camera rotation
		{
			DirectX::XMUINT2 mousePosition;
			inputs.GetMousePosition(mousePosition.x, mousePosition.y);
			if (m_firstFrameMouseDown)
			{
				m_mousePreviousPos = mousePosition;
				m_firstFrameMouseDown = false;
			}

			DirectX::XMINT2 delta;
			delta.x = m_mousePreviousPos.x - mousePosition.x;
			delta.y = m_mousePreviousPos.y - mousePosition.y;

			if (delta.x != 0 || delta.y != 0)
			{
				DirectX::XMVECTOR eulerRotation = DirectX::XMVectorSet(-static_cast<float>(delta.y) * m_rotationSpeed * dtInSeconds, -static_cast<float>(delta.x) * m_rotationSpeed * dtInSeconds, 0, 0);
				m_cameraEulerAngle += eulerRotation;

				DirectX::XMMATRIX YRotation = DirectX::XMMatrixRotationY(m_cameraEulerAngle.m128_f32[1]);
				DirectX::XMMATRIX XRotation = DirectX::XMMatrixRotationX(m_cameraEulerAngle.m128_f32[0]);

				m_cameraRotation = DirectX::XMMatrixMultiply(XRotation, YRotation);
				m_mousePreviousPos = mousePosition;
				updateCameraTransform = true;
			}
		}

		if (updateCameraTransform)
		{
			DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(m_cameraPosition);
			m_cameraTransform = DirectX::XMMatrixMultiply(m_cameraRotation, translationMatrix);
		}
	}
}
