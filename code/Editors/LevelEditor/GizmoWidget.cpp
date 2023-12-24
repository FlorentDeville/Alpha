/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/GizmoWidget.h"

#include "Core/Math/Aabb.h"
#include "Core/Math/Ray.h"
#include "Core/Math/Vec4f.h"

#include "Editors/LevelEditor/LevelEditor.h"
#include "Editors/LevelEditor/GizmoModel.h"

#include "Inputs/InputMgr.h"

#include "Rendering/RenderModule.h"

using namespace DirectX;

//#pragma optimize("", off)

namespace Editors
{
	GizmoWidget::GizmoWidget()
		: m_manipulatorMode(kTranslation)
		, m_pModel(nullptr)
		, BASE_DIAMETER(0.075f)
		, LENGTH(7.f)
		, CONE_BASE_DIAMETER(0.5)
		, CONE_BASE_LENGTH(0.75)
		, ROTATION_DIAMATER(5.f)
		, SCALE_SQUARE_SIZE(0.5f)
#if defined(DEBUG_COLLISION)
		, m_debugCollisionDetected(false)
#endif
	{
		m_txWs = DirectX::XMMatrixIdentity();
	}

	GizmoWidget::~GizmoWidget()
	{
		if (m_pModel)
		{
			delete m_pModel;
			m_pModel = nullptr;
		}
	}

	void GizmoWidget::Update(const DirectX::XMVECTOR& mouse3dPosition)
	{
		//create ray 
		const Core::Mat44f& cameraWs = LevelEditor::Get().GetCameraWs();
		
		Core::Vec4f mouseWs(mouse3dPosition.m128_f32[0], mouse3dPosition.m128_f32[1], mouse3dPosition.m128_f32[2], mouse3dPosition.m128_f32[3]);

		const Core::Vec4f& rayOrigin = cameraWs.GetT();
		Core::Vec4f rayDirection = mouseWs - rayOrigin;
		rayDirection.Set(3, 0);
		rayDirection.Normalize();
		Core::Ray mousePickingRay(rayOrigin, rayDirection);

#if defined(DEBUG_RAY)
		if (Inputs::InputMgr::Get().IsMouseLeftButtonDown())
		{
			m_debugRay = mousePickingRay;
		}
#endif

		//create aabb for x axis
		Core::Vec4f objectPosition(m_txWs.r[3].m128_f32[0], m_txWs.r[3].m128_f32[1], m_txWs.r[3].m128_f32[2], 1);
		float size = ComputeConstantScreenSizeScale(objectPosition);
		float realLength = LENGTH * size;

		const float BOX_HALF_SIZE = 0.5f;
		Core::Vec4f min(0, -BOX_HALF_SIZE, -BOX_HALF_SIZE, 0);
		Core::Vec4f max(realLength, BOX_HALF_SIZE, BOX_HALF_SIZE, 0);
		Core::Aabb axisAabb(min, max);

		//convert ray to gizmo local space
		Core::Mat44f txWs(m_txWs);
		Core::Mat44f invTxWs = txWs.Inverse();

		mousePickingRay.Transform(invTxWs);
		
		//collision test, ray vs aabb
#if defined(DEBUG_COLLISION)
		m_debugCollisionDetected = false;
#endif

		{
			float tmin = 0;
			float tmax = FLT_MAX;
			Core::Vec4f invRayDirectionLs = mousePickingRay.ComputeInverseDirection();
			for (int ii = 0; ii < 3; ++ii)
			{
				float t1 = (axisAabb.GetMin().Get(ii) - mousePickingRay.GetOrigin().Get(ii)) * invRayDirectionLs.Get(ii);
				float t2 = (axisAabb.GetMax().Get(ii) - mousePickingRay.GetOrigin().Get(ii)) * invRayDirectionLs.Get(ii);

				tmin = std::min(std::max(t1, tmin), std::max(t2, tmin));
				tmax = std::max(std::min(t1, tmax), std::min(t2, tmax));
			}

			if (tmin < tmax)
			{
#if defined(DEBUG_COLLISION)
				m_debugCollisionDetected = true;
#endif
			}
		}
	}

	void GizmoWidget::Render()
	{
#if defined(DEBUG_RAY)
		for (int ii = 0; ii < 100; ++ii)
		{
			Core::Vec4f pos = m_debugRay.GetOrigin() + m_debugRay.GetDirection() * (float)ii;

			DirectX::XMVECTOR scale = DirectX::XMVectorSet(0.1f, 0.1f, 0.1f, 0);
			DirectX::XMVECTOR translation = DirectX::XMVectorSet(pos.GetX(), pos.GetY(), pos.GetZ(), 1);
			DirectX::XMMATRIX txWs = DirectX::XMMatrixAffineTransformation(scale, DirectX::g_XMZero, DirectX::g_XMIdentityR3, translation);

			Rendering::RenderModule::Get().RenderPrimitiveCube(txWs, DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1));
		}
#endif

		switch (m_manipulatorMode)
		{
		case kTranslation:
			RenderTranslationManipulator();
			break;

		case kRotation:
			RenderRotationManipulator();
			break;

		case kScale:
			RenderScaleManipulator();
			break;

		default:
			break;
		}
	}

	void GizmoWidget::SetModel(GizmoModel* pModel)
	{
		if (m_pModel == pModel)
			return;

		if (m_pModel)
			delete m_pModel;

		m_pModel = pModel;

		if (!pModel)
			return;

		pModel->OnNodeChanged([this](Node* pNode) { OnNodeChanged_Model(pNode); });
		m_txWs = pModel->GetTransform();
	}

	void GizmoWidget::SetManipulatorMode(ManipulatorMode mode)
	{
		m_manipulatorMode = mode;
	}

	void GizmoWidget::RenderRotationManipulator()
	{
		Core::Vec4f objectPosition(m_txWs.r[3].m128_f32[0], m_txWs.r[3].m128_f32[1], m_txWs.r[3].m128_f32[2], 1);
		float size = ComputeConstantScreenSizeScale(objectPosition);

		float realRotationDiameter = ROTATION_DIAMATER * size;
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(realRotationDiameter, realRotationDiameter, realRotationDiameter);

		//rotation x axis
		{
			constexpr float rotationAngle = DirectX::XMConvertToRadians(90.f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(0, 0, rotationAngle);
			DirectX::XMMATRIX mvpMatrix = scale * rotation * m_txWs;

			DirectX::XMFLOAT4 red(1, 0, 0, 1);
			Rendering::RenderModule::Get().RenderPrimitiveTorus(mvpMatrix, red);
		}

		//rotation y axis
		{
			DirectX::XMFLOAT4 green(0, 1, 0, 1);

			DirectX::XMMATRIX mvpMatrix = scale * m_txWs;
			Rendering::RenderModule::Get().RenderPrimitiveTorus(mvpMatrix, green);
		}

		//rotation z axis
		{
			constexpr float rotationAngle = DirectX::XMConvertToRadians(90.f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(rotationAngle, 0, 0);
			DirectX::XMMATRIX mvpMatrix = scale * rotation * m_txWs;

			DirectX::XMFLOAT4 blue(0, 0, 1, 1);
			Rendering::RenderModule::Get().RenderPrimitiveTorus(mvpMatrix, blue);
		}
	}

	void GizmoWidget::RenderTranslationManipulator()
	{
		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

		//x axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(-DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 red(1, 0, 0, 0);
			RenderTranslationSingleAxis(rotation * m_txWs, red);
		}

		//y axis
		{
			DirectX::XMFLOAT4 green(0, 1, 0, 0);
			RenderTranslationSingleAxis(m_txWs, green);
		}

		//z axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 blue(0, 0, 1, 0);
			RenderTranslationSingleAxis(rotation * m_txWs, blue);
		}
	}

	void GizmoWidget::RenderScaleManipulator()
	{
		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

		//x axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(-DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 red(1, 0, 0, 0);
			RenderScaleSingleAxis(rotation * m_txWs, red);
		}

		//y axis
		{
			DirectX::XMFLOAT4 green(0, 1, 0, 0);
			RenderScaleSingleAxis(m_txWs, green);
		}

		//z axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 blue(0, 0, 1, 0);
			RenderScaleSingleAxis(rotation * m_txWs, blue);
		}
	}

	void GizmoWidget::RenderTranslationSingleAxis(const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color)
	{
		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();
		
		Core::Vec4f objectPosition(txWs.r[3].m128_f32[0], txWs.r[3].m128_f32[1], txWs.r[3].m128_f32[2], 1);
		float size = ComputeConstantScreenSizeScale(objectPosition);

		float realDiameter = BASE_DIAMETER * size;
		float realLength = LENGTH * size;

		//debug render aabb
#if defined(DEBUG_COLLISION)
		{
			DirectX::XMVECTOR scale = DirectX::XMVectorSet(realLength, 1, 1, 0);
			DirectX::XMVECTOR translation = m_txWs.r[3];
			translation.m128_f32[0] += (realLength * 0.5f);
			DirectX::XMMATRIX txWs = DirectX::XMMatrixAffineTransformation(scale, DirectX::g_XMZero, DirectX::g_XMIdentityR3, translation);

			Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

			DirectX::XMFLOAT4 color(1, 1, 0, 1);
			if(m_debugCollisionDetected)
				color = DirectX::XMFLOAT4(0.99f, 0.49f, 0.22f, 1);

			renderingMgr.RenderPrimitiveCube(txWs, color);
		}
#endif

		{
			DirectX::XMVECTOR scale = DirectX::XMVectorSet(realDiameter, realLength, realDiameter, 0);
			DirectX::XMVECTOR translation = DirectX::XMVectorSet(0.f, realLength * 0.5f, 0, 0);
			DirectX::XMMATRIX txLs = DirectX::XMMatrixAffineTransformation(scale, DirectX::g_XMZero, DirectX::g_XMIdentityR3, translation);
			DirectX::XMMATRIX ws = txLs * txWs;
			renderingMgr.RenderPrimitiveCylinder(ws, color);
		}

		{
			
			float coneDiameter = CONE_BASE_DIAMETER * size;
			float coneLength = CONE_BASE_LENGTH * size;

			DirectX::XMVECTOR localTranslation = DirectX::XMVectorSet(0, realLength, 0, 0);
			DirectX::XMVECTOR localRotationOrigin = DirectX::XMVectorSet(0, 0, 0, 1);
			DirectX::XMVECTOR localRotation = DirectX::XMQuaternionIdentity();
			DirectX::XMVECTOR localScale = DirectX::XMVectorSet(coneDiameter, coneLength, coneDiameter, 0);
			DirectX::XMMATRIX txLs = DirectX::XMMatrixAffineTransformation(localScale, localRotationOrigin, localRotation, localTranslation);

			DirectX::XMMATRIX ws = txLs * txWs;
			renderingMgr.RenderPrimitiveCone(ws, color);
		}
	}

	void GizmoWidget::RenderScaleSingleAxis(const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color) const
	{
		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

		Core::Vec4f objectPosition(txWs.r[3].m128_f32[0], txWs.r[3].m128_f32[1], txWs.r[3].m128_f32[2], 1);
		float size = ComputeConstantScreenSizeScale(objectPosition);

		float realDiameter = BASE_DIAMETER * size;
		float realLength = LENGTH * size;

		{
			DirectX::XMVECTOR scale = DirectX::XMVectorSet(realDiameter, realLength, realDiameter, 0);
			DirectX::XMVECTOR translation = DirectX::XMVectorSet(0.f, realLength * 0.5f, 0, 0);
			DirectX::XMMATRIX txLs = DirectX::XMMatrixAffineTransformation(scale, DirectX::g_XMZero, DirectX::g_XMIdentityR3, translation);
			DirectX::XMMATRIX ws = txLs * txWs;
			renderingMgr.RenderPrimitiveCylinder(ws, color);
		}

		{

			float coneDiameter = CONE_BASE_DIAMETER * size;
			float coneLength = SCALE_SQUARE_SIZE * size;

			DirectX::XMVECTOR localTranslation = DirectX::XMVectorSet(0, realLength, 0, 0);
			DirectX::XMVECTOR localRotationOrigin = DirectX::XMVectorSet(0, 0, 0, 1);
			DirectX::XMVECTOR localRotation = DirectX::XMQuaternionIdentity();
			DirectX::XMVECTOR localScale = DirectX::XMVectorSet(coneLength, coneLength, coneLength, 0);
			DirectX::XMMATRIX txLs = DirectX::XMMatrixAffineTransformation(localScale, localRotationOrigin, localRotation, localTranslation);

			DirectX::XMMATRIX ws = txLs * txWs;
			renderingMgr.RenderPrimitiveCube(ws, color);
		}
	}

	float GizmoWidget::ComputeConstantScreenSizeScale(const Core::Vec4f& objectPosition) const
	{
		const LevelEditor& levelEditor = LevelEditor::Get();
		const Core::Mat44f& camera = levelEditor.GetCameraWs();
		float fov = levelEditor.GetFovRad();

		Core::Vec4f dt = camera.GetT() - objectPosition;
		float distance = dt.Length();
		float worldSize = (2 * tanf(fov * 0.5f)) * distance;
		const float SCREEN_RATIO = 0.025f;
		float size = SCREEN_RATIO * worldSize;
		return size;
	}

	void GizmoWidget::OnNodeChanged_Model(Node* pNode)
	{
		m_txWs = m_pModel->GetTransform();
	}
}