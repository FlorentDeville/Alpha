/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

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
		, m_view()
		, m_proj()
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
		Core::Vec4f cameraUp(0, 1, 0, 0);
		Core::Vec4f targetOffset(0, 0, 1, 1);

		Core::Mat44f cameraWorld;
		cameraWorld.m_matrix = m_cameraTransform;
		Core::Vec4f cameraLookAt = targetOffset * cameraWorld;

		m_view = Core::Mat44f::CreateView(m_cameraPosition, cameraLookAt - m_cameraPosition, cameraUp);

		const float nearDistance = 0.1f;
		const float farDistance = 1000.f;
		m_proj = Core::Mat44f::CreatePerspective(m_fov, aspectRatio, nearDistance, farDistance);
	}

	void CameraWidget::SetTransform(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& eulerAngle)
	{
		m_cameraPosition = pos;
		m_cameraEulerAngle = eulerAngle;

		Core::Vec4f cameraPosition(m_cameraPosition);
		Core::Vec4f cameraEulerAngle(m_cameraEulerAngle);

		Core::Mat44f XRotation = Core::Mat44f::CreateRotationX(cameraEulerAngle.GetX());
		Core::Mat44f YRotation = Core::Mat44f::CreateRotationY(cameraEulerAngle.GetY());

		Core::Mat44f rotXY = XRotation * YRotation;
		m_cameraRotation = rotXY.m_matrix;
		
		Core::Mat44f translation = Core::Mat44f::CreateTranslationMatrix(cameraPosition);
		Core::Mat44f transform = rotXY * translation;

		m_cameraTransform = transform.m_matrix;

		m_onWsChanged(transform);
	}

	void CameraWidget::SetFov(float fov)
	{
		m_fov = fov;
	}

	const Core::Mat44f& CameraWidget::GetView() const
	{
		return m_view;
	}

	const Core::Mat44f& CameraWidget::GetProjection() const
	{
		return m_proj;
	}

	const Core::Vec4f& CameraWidget::GetPosition() const
	{
		return m_cameraPosition;
	}

	float CameraWidget::GetFov() const
	{
		return m_fov;
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
			m_cameraPosition = m_cameraPosition + (zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('S')) //backward
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[2];
			m_cameraPosition = m_cameraPosition - (zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('A')) //left
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[0];
			m_cameraPosition = m_cameraPosition - (zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('D')) //right
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[0];
			m_cameraPosition = m_cameraPosition + (zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('Q')) //up
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[1];
			m_cameraPosition = m_cameraPosition + (zAxis * m_translationSpeed * dtInSeconds);
			updateCameraTransform = true;
		}
		if (inputs.IsKeyPressed('E')) //down
		{
			DirectX::XMVECTOR zAxis = m_cameraRotation.r[1];
			m_cameraPosition = m_cameraPosition - (zAxis * m_translationSpeed * dtInSeconds);
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
			Core::Mat44f translation = Core::Mat44f::CreateTranslationMatrix(m_cameraPosition);
			Core::Mat44f rotation;
			rotation.m_matrix = m_cameraRotation;

			Core::Mat44f transform = rotation * translation;
			m_cameraTransform = transform.m_matrix;
			m_onWsChanged(transform);
		}

		m_mousePreviousPos = mousePosition;
	}
}
