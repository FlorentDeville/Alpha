/********************************************************************/
/* � 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/Widgets/CameraWidget.h"

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/LevelEditorModule.h"

#include "Inputs/InputMgr.h"

#include "OsWin/Input.h"

#include "Rendering/Camera.h"
#include "Rendering/RenderModule.h"

//needed for the operators *-+ between vectors and floats.
using namespace DirectX;

namespace Editors
{
	CameraWidget::CameraWidget()
		: m_firstFrameMouseDown(true)
		, m_mousePreviousPos()
		, m_cameraState(CameraState::None)
		, m_translationSpeed(10.f)
		, m_rotationSpeed(0.3f)
		, m_cameraEulerAngle()
		, m_cameraPosition()
		, m_cameraRotation()
		, m_cameraTransform()
	{}

	CameraWidget::~CameraWidget()
	{}

	void CameraWidget::Update(float dtInSeconds)
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

	void CameraWidget::Render(float aspectRatio)
	{
		float fovRad = LevelEditorModule::Get().GetFovRad();

		Rendering::RenderModule& renderModule = Rendering::RenderModule::Get();
		Rendering::Camera* pCamera = renderModule.GetCamera();

		DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 1);
		DirectX::XMVECTOR targetOffset = DirectX::XMVectorSet(0, 0, 1, 1);
		DirectX::XMVECTOR cameraLookAt = DirectX::XMVector4Transform(targetOffset, m_cameraTransform);
		DirectX::XMVECTOR cameraPosition = m_cameraTransform.r[3];
		pCamera->SetLookAt(cameraPosition, cameraLookAt, cameraUp);

		const float nearDistance = 0.1f;
		const float farDistance = 1000.f;
		pCamera->SetProjection(fovRad, aspectRatio, nearDistance, farDistance);
	}

	void CameraWidget::SetTransform(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& eulerAngle)
	{
		m_cameraPosition = pos;
		m_cameraEulerAngle = eulerAngle;

		DirectX::XMMATRIX YRotation = DirectX::XMMatrixRotationY(m_cameraEulerAngle.m128_f32[1]);
		DirectX::XMMATRIX XRotation = DirectX::XMMatrixRotationX(m_cameraEulerAngle.m128_f32[0]);
		m_cameraRotation = XRotation * YRotation;
		
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(m_cameraPosition);
		m_cameraTransform = m_cameraRotation * translationMatrix;

		const DirectX::XMVECTOR& dxX = m_cameraTransform.r[0];
		const DirectX::XMVECTOR& dxY = m_cameraTransform.r[1];
		const DirectX::XMVECTOR& dxZ = m_cameraTransform.r[2];
		const DirectX::XMVECTOR& dxW = m_cameraTransform.r[3];
		Core::Vec4f x(dxX.m128_f32[0], dxX.m128_f32[1], dxX.m128_f32[2], dxX.m128_f32[3]);
		Core::Vec4f y(dxY.m128_f32[0], dxY.m128_f32[1], dxY.m128_f32[2], dxY.m128_f32[3]);
		Core::Vec4f z(dxZ.m128_f32[0], dxZ.m128_f32[1], dxZ.m128_f32[2], dxZ.m128_f32[3]);
		Core::Vec4f w(dxW.m128_f32[0], dxW.m128_f32[1], dxW.m128_f32[2], dxW.m128_f32[3]);
		Core::Mat44f mat(x, y, z, w);
		m_onWsChanged(mat);
	}

	Core::CallbackId CameraWidget::OnWsChanged(const OnWsChangedEvent::Callback& callback)
	{
		return m_onWsChanged.Connect(callback);
	}

	void CameraWidget::UpdateCamera_None(float dtInSeconds)
	{
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();
		if (inputs.IsMouseRightButtonDown())
			m_cameraState = CameraState::FPS;
	}

	void CameraWidget::UpdateCamera_FPS(float dtInSeconds)
	{
		Inputs::InputMgr& inputs = Inputs::InputMgr::Get();

		DirectX::XMINT2 mousePosition;
		Os::GetMousePosition(mousePosition.x, mousePosition.y);

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

			if (m_translationSpeed < 1) m_translationSpeed = 1.f;
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
		}
		if (inputs.IsKeyPressed('D')) //right
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[0];
			m_cameraPosition = DirectX::XMVectorAdd(m_cameraPosition, zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
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
			if (m_firstFrameMouseDown)
			{
				m_mousePreviousPos = mousePosition;
				m_firstFrameMouseDown = false;
			}

			DirectX::XMINT2 mouseDelta;
			mouseDelta.x = m_mousePreviousPos.x - mousePosition.x;
			mouseDelta.y = m_mousePreviousPos.y - mousePosition.y;

			if (mouseDelta.x != 0 || mouseDelta.y != 0)
			{
				float x = -static_cast<float>(mouseDelta.y) * m_rotationSpeed * dtInSeconds;
				float y = -static_cast<float>(mouseDelta.x) * m_rotationSpeed * dtInSeconds;
				
				DirectX::XMVECTOR eulerRotation = DirectX::XMVectorSet(x, y, 0, 0);
				m_cameraEulerAngle += eulerRotation;
				
				DirectX::XMMATRIX YRotation = DirectX::XMMatrixRotationY(m_cameraEulerAngle.m128_f32[1]);
				DirectX::XMMATRIX XRotation = DirectX::XMMatrixRotationX(m_cameraEulerAngle.m128_f32[0]);

				m_cameraRotation = DirectX::XMMatrixMultiply(XRotation, YRotation);

				updateCameraTransform = true;
			}
		}

		if (updateCameraTransform)
		{
			DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(m_cameraPosition);
			m_cameraTransform = m_cameraRotation * translationMatrix;

			const DirectX::XMVECTOR& dxX = m_cameraTransform.r[0];
			const DirectX::XMVECTOR& dxY = m_cameraTransform.r[1];
			const DirectX::XMVECTOR& dxZ = m_cameraTransform.r[2];
			const DirectX::XMVECTOR& dxW = m_cameraTransform.r[3];
			Core::Vec4f x(dxX.m128_f32[0], dxX.m128_f32[1], dxX.m128_f32[2], dxX.m128_f32[3]);
			Core::Vec4f y(dxY.m128_f32[0], dxY.m128_f32[1], dxY.m128_f32[2], dxY.m128_f32[3]);
			Core::Vec4f z(dxZ.m128_f32[0], dxZ.m128_f32[1], dxZ.m128_f32[2], dxZ.m128_f32[3]);
			Core::Vec4f w(dxW.m128_f32[0], dxW.m128_f32[1], dxW.m128_f32[2], dxW.m128_f32[3]);
			Core::Mat44f mat(x, y, z, w);
			m_onWsChanged(mat);
		}

		m_mousePreviousPos = mousePosition;
	}
}
