/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/GizmoWidget.h"

#include "Core/Math/Aabb.h"
#include "Core/Math/Intersection.h"
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
		, m_internalState(InternalState::kIdle)
		, m_previousMousePosition()
		, m_translationOffset()
		, m_hoverAxis()
		, m_hoverColor(0.99f, 0.49f, 0.22f, 1)
	{
		m_txWs.SetIdentity();
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
		switch (m_internalState)
		{
		case InternalState::kIdle:
			UpdateState_Idle(mouse3dPosition);
			break;

		case InternalState::kMoving:
			UpdateState_Moving(mouse3dPosition);
			break;

		default:
			break;
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

		Core::Vec4f xAxis = m_txWs.GetX();
		Core::Vec4f yAxis = m_txWs.GetY();
		Core::Vec4f zAxis = m_txWs.GetZ();
		Core::Vec4f tAxis(0, 0, 0, 1);

		xAxis.Normalize();
		yAxis.Normalize();
		zAxis.Normalize();

		Core::Mat44f rotationMatrix(xAxis, yAxis, zAxis, tAxis);

		m_eulerAngles = rotationMatrix.GetEulerAngle();
	}

	void GizmoWidget::SetManipulatorMode(ManipulatorMode mode)
	{
		m_manipulatorMode = mode;
	}

	void GizmoWidget::UpdateState_Idle(const DirectX::XMVECTOR& mouse3dPosition)
	{
		UpdateMouseHover(mouse3dPosition);

		if (!m_hoverAxis.IsEmpty())
		{
			if (Inputs::InputMgr::Get().IsMouseLeftButtonDown())
			{
				m_internalState = InternalState::kMoving;
				Inputs::InputMgr::Get().GetMousePosition(m_previousMousePosition.x, m_previousMousePosition.y);

				if (m_manipulatorMode == kTranslation)
				{
					//project the axis in screen space
					int axisIndex = m_hoverAxis.GetAxisIndex();
					
					const Core::Vec4f& axis = m_txWs.GetRow(axisIndex);

					{
						Core::Vec4f mouse3dPos(mouse3dPosition.m128_f32[0], mouse3dPosition.m128_f32[1], mouse3dPosition.m128_f32[2], mouse3dPosition.m128_f32[3]);
						const Core::Vec4f& A = LevelEditor::Get().GetCameraWs().GetT();
						Core::Vec4f aDir = mouse3dPos - A;
						aDir.Normalize();

						Core::Vec4f B = m_txWs.GetT();
						Core::Vec4f bDir = axis;

						Core::Vec4f closestPointOnAxis;
						Core::Intersection::RayVsRay_ClosestPoint(Core::Ray(A, aDir), Core::Ray(B, bDir), closestPointOnAxis);
						
						m_translationOffset = closestPointOnAxis - B;
					}
				}
				else if (m_manipulatorMode == kRotation)
				{
					Core::Vec4f xAxis = m_txWs.GetX();
					Core::Vec4f yAxis = m_txWs.GetY();
					Core::Vec4f zAxis = m_txWs.GetZ();
					Core::Vec4f tAxis(0, 0, 0, 1);
					Core::Mat44f txWs(xAxis, yAxis, zAxis, tAxis);
					m_rotationInitialEulerAngles = txWs.GetEulerAngle();
				}
			}
		}
	}

	void GizmoWidget::UpdateState_Moving(const DirectX::XMVECTOR& mouse3dPosition)
	{
		if (!Inputs::InputMgr::Get().IsMouseLeftButtonDown())
			m_internalState = InternalState::kIdle;

		if (m_manipulatorMode == kTranslation)
		{
			UpdateState_Moving_Translation(mouse3dPosition);
		}
		else if (m_manipulatorMode == kRotation)
		{
			UpdateState_Moving_Rotation(mouse3dPosition);
		}
	}

	void GizmoWidget::UpdateMouseHover(const DirectX::XMVECTOR& mouse3dPosition)
	{
		m_hoverAxis = GizmoAxis::GizmoAxisEnum::kEmpty;

		switch (m_manipulatorMode)
		{
		case kTranslation:
			UpdateMouseHoverTranslation(mouse3dPosition);
			break;

		case kRotation:
			UpdateMouseHoverRotation(mouse3dPosition);
			break;

		case kScale:
			//RenderScaleManipulator();
			break;

		default:
			break;
		}
	}

	void GizmoWidget::UpdateMouseHoverTranslation(const DirectX::XMVECTOR& mouse3dPosition)
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

		//convert ray to gizmo local space
		Core::Mat44f txWs(m_txWs);
		Core::Mat44f invTxWs = txWs.Inverse();

		Core::Ray mousePickingRayLs = mousePickingRay;
		mousePickingRayLs.Transform(invTxWs);

		//compute aabb size
		Core::Vec4f objectPosition = m_txWs.GetT();
		float size = ComputeConstantScreenSizeScale(objectPosition);
		float realLength = LENGTH * size;
		const float BOX_HALF_SIZE = 0.5f * size;

		//x axis
		{
			
			Core::Vec4f min(0, -BOX_HALF_SIZE, -BOX_HALF_SIZE, 0);
			Core::Vec4f max(realLength, BOX_HALF_SIZE, BOX_HALF_SIZE, 0);
			Core::Aabb axisAabb(min, max);

			//collision test, ray vs aabb
			bool collision = Core::Intersection::RayVsAabb(mousePickingRayLs, axisAabb);
			if (collision)
			{
				m_hoverAxis = GizmoAxis::GizmoAxisEnum::kXAxis;
				return;
			}
		}

		//y axis
		{
			//create aabb for x axis
			Core::Vec4f min(-BOX_HALF_SIZE, 0, -BOX_HALF_SIZE, 0);
			Core::Vec4f max(BOX_HALF_SIZE, realLength, BOX_HALF_SIZE, 0);
			Core::Aabb axisAabb(min, max);

			//collision test, ray vs aabb
			bool collision = Core::Intersection::RayVsAabb(mousePickingRayLs, axisAabb);
			if (collision)
			{
				m_hoverAxis = GizmoAxis::GizmoAxisEnum::kYAxis;
				return;
			}
		}

		//z axis
		{
			//create aabb for x axis
			Core::Vec4f min(-BOX_HALF_SIZE, -BOX_HALF_SIZE, 0, 0);
			Core::Vec4f max(BOX_HALF_SIZE, BOX_HALF_SIZE, realLength, 0);
			Core::Aabb axisAabb(min, max);

			//collision test, ray vs aabb
			bool collision = Core::Intersection::RayVsAabb(mousePickingRayLs, axisAabb);
			if (collision)
			{
				m_hoverAxis = GizmoAxis::GizmoAxisEnum::kZAxis;
				return;
			}
		}
	}

	void GizmoWidget::UpdateMouseHoverRotation(const DirectX::XMVECTOR& mouse3dPosition)
	{
		//create ray 
		const Core::Mat44f& cameraWs = LevelEditor::Get().GetCameraWs();

		Core::Vec4f mouseWs(mouse3dPosition.m128_f32[0], mouse3dPosition.m128_f32[1], mouse3dPosition.m128_f32[2], mouse3dPosition.m128_f32[3]);

		const Core::Vec4f& rayOrigin = cameraWs.GetT();
		Core::Vec4f rayDirection = mouseWs - rayOrigin;
		rayDirection.Set(3, 0);
		rayDirection.Normalize();
		Core::Ray mousePickingRay(rayOrigin, rayDirection);

		//disk center
		Core::Vec4f diskCenter = m_txWs.GetT();

		//disk inner and outer radius
		float size = ComputeConstantScreenSizeScale(diskCenter);

		float torusRadius = ROTATION_DIAMATER * size;
		float smallRadius = 1.f * size;

		float outerRadius = torusRadius + smallRadius;
		float innerRadius = torusRadius - smallRadius;

		float smallestParameter = FLT_MAX;
		int closestAxisIndex = -1;

		for (int axisIndex = 0; axisIndex < 3; ++axisIndex)
		{
			//create the disk info
			Core::Vec4f diskNormal = m_txWs.GetRow(axisIndex);

			float parameter = 0;
			bool collision = Core::Intersection::RayVsDisk(mousePickingRay, diskNormal, diskCenter, innerRadius, outerRadius, parameter);
			if (collision && parameter < smallestParameter)
			{
				smallestParameter = parameter;
				closestAxisIndex = axisIndex;
			}
		}

		if (closestAxisIndex != -1)
		{
			m_hoverAxis.SetAxisIndex(closestAxisIndex);
			m_rotationInitialPoint = mousePickingRay.GetOrigin() + mousePickingRay.GetDirection() * smallestParameter;
		}

	}

	void GizmoWidget::UpdateState_Moving_Translation(const DirectX::XMVECTOR& mouse3dPosition)
	{
		//project the axis in screen space
		int axisIndex = m_hoverAxis.GetAxisIndex();

		const Core::Vec4f& axis = m_txWs.GetRow(axisIndex);

		{
			Core::Vec4f mouse3dPos(mouse3dPosition.m128_f32[0], mouse3dPosition.m128_f32[1], mouse3dPosition.m128_f32[2], mouse3dPosition.m128_f32[3]);
			const Core::Vec4f& A = LevelEditor::Get().GetCameraWs().GetT();
			Core::Vec4f aDir = mouse3dPos - A;
			aDir.Normalize();

			Core::Vec4f B = m_txWs.GetT();
			Core::Vec4f bDir = axis;

			Core::Vec4f closestPointOnAxis;
			Core::Intersection::RayVsRay_ClosestPoint(Core::Ray(A, aDir), Core::Ray(B, bDir), closestPointOnAxis);

			closestPointOnAxis = closestPointOnAxis - m_translationOffset;
			closestPointOnAxis.Set(3, 1);
			m_txWs.SetRow(3, closestPointOnAxis);

			m_pModel->Translate(m_txWs);
		}
	}

	void GizmoWidget::UpdateState_Moving_Rotation(const DirectX::XMVECTOR& mouse3dPosition)
	{
		//get the closest point from the ray to the disk
		Core::Vec4f mouse3dPos(mouse3dPosition.m128_f32[0], mouse3dPosition.m128_f32[1], mouse3dPosition.m128_f32[2], mouse3dPosition.m128_f32[3]);
		const Core::Vec4f& rayOrigin = LevelEditor::Get().GetCameraWs().GetT();
		Core::Vec4f rayDir = mouse3dPos - rayOrigin;
		rayDir.Normalize();

		//disk center
		Core::Vec4f diskCenter = m_txWs.GetT();

		//disk axis
		int axisIndex = m_hoverAxis.GetAxisIndex();
		Core::Vec4f diskNormal = m_txWs.GetRow(axisIndex);

		Core::Vec4f closestPoint;
		Core::Intersection::RayVsCircle_ClosestPoint(Core::Ray(rayOrigin, rayDir), diskNormal, diskCenter, 1.f, closestPoint);
		Core::Vec4f currentVector = closestPoint - diskCenter;
		currentVector.Normalize();

		//compute angle between start point and current point
		Core::Vec4f initialVector = m_rotationInitialPoint - diskCenter;
		initialVector.Normalize();

		float cos = currentVector.Dot(initialVector);

		//saturate cos between 1 and -1.
		if (cos > 1) cos = 1;
		if (cos < -1) cos = -1;

		Core::Vec4f cross = currentVector.Cross(initialVector);
		float sin = cross.Dot(diskNormal);

		char buffer[64];
		snprintf(buffer, 64, "cos %f, sin %f\n", cos, sin);
		OutputDebugString(buffer);

		float angle = acosf(cos);
		if (angle == 0)
			return;

		if (sin < 0)
			angle = DirectX::XM_2PI - angle;
		
		snprintf(buffer, 64, "angle %f\n", angle);
		OutputDebugString(buffer);

		Core::Vec4f currentEulerAngle(0, 0, 0, 0);
		currentEulerAngle.Set(axisIndex, angle);
		Core::Vec4f eulerAngles = m_rotationInitialEulerAngles + currentEulerAngle;

		float scaleX = m_txWs.GetX().Length();
		float scaleY = m_txWs.GetY().Length();
		float scaleZ = m_txWs.GetZ().Length();

		Core::Mat44f scaleMatrix(
			Core::Vec4f(scaleX, 0, 0, 0),
			Core::Vec4f(0, scaleY, 0, 0),
			Core::Vec4f(0, 0, scaleZ, 0),
			Core::Vec4f(0, 0, 0, 1)
		);

		Core::Mat44f rotationMatrix = Core::Mat44f::CreateRotationMatrixFromEulerAngles(eulerAngles);

		Core::Mat44f translationMatrix;
		translationMatrix.SetIdentity();
		translationMatrix.SetRow(3, diskCenter);

		m_txWs = scaleMatrix * rotationMatrix * translationMatrix;

		m_pModel->Translate(m_txWs);
	}

	void GizmoWidget::RenderRotationManipulator()
	{
		const Core::Vec4f& dxXAxis = m_txWs.GetX();
		const Core::Vec4f& dxYAxis = m_txWs.GetY();
		const Core::Vec4f& dxZAxis = m_txWs.GetZ();
		const Core::Vec4f& dxTAxis = m_txWs.GetT();
		DirectX::XMVECTOR xAxis = DirectX::XMVectorSet(dxXAxis.GetX(), dxXAxis.GetY(), dxXAxis.GetZ(), 0);
		DirectX::XMVECTOR yAxis = DirectX::XMVectorSet(dxYAxis.GetX(), dxYAxis.GetY(), dxYAxis.GetZ(), 0);
		DirectX::XMVECTOR zAxis = DirectX::XMVectorSet(dxZAxis.GetX(), dxZAxis.GetY(), dxZAxis.GetZ(), 0);
		DirectX::XMVECTOR tAxis = DirectX::XMVectorSet(dxTAxis.GetX(), dxTAxis.GetY(), dxTAxis.GetZ(), 1);
		DirectX::XMMATRIX dxTxWs(xAxis, yAxis, zAxis, tAxis);

		Core::Vec4f objectPosition = m_txWs.GetT();
		float size = ComputeConstantScreenSizeScale(objectPosition);

		float realRotationDiameter = ROTATION_DIAMATER * size;
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(realRotationDiameter, realRotationDiameter, realRotationDiameter);

		//rotation x axis
		{
			constexpr float rotationAngle = DirectX::XMConvertToRadians(90.f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(0, 0, rotationAngle);
			DirectX::XMMATRIX mvpMatrix = scale * rotation * dxTxWs;

			DirectX::XMFLOAT4 red(1, 0, 0, 1);
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kXAxis))
				red = m_hoverColor;

			Rendering::RenderModule::Get().RenderPrimitiveTorus(mvpMatrix, red);
		}

		//rotation y axis
		{
			DirectX::XMFLOAT4 green(0, 1, 0, 1);
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kYAxis))
				green = m_hoverColor;

			DirectX::XMMATRIX mvpMatrix = scale * dxTxWs;
			Rendering::RenderModule::Get().RenderPrimitiveTorus(mvpMatrix, green);
		}

		//rotation z axis
		{
			constexpr float rotationAngle = DirectX::XMConvertToRadians(90.f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(rotationAngle, 0, 0);
			DirectX::XMMATRIX mvpMatrix = scale * rotation * dxTxWs;

			DirectX::XMFLOAT4 blue(0, 0, 1, 1);
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kZAxis))
				blue = m_hoverColor;

			Rendering::RenderModule::Get().RenderPrimitiveTorus(mvpMatrix, blue);
		}
	}

	void GizmoWidget::RenderTranslationManipulator()
	{
		const Core::Vec4f& dxXAxis = m_txWs.GetX();
		const Core::Vec4f& dxYAxis = m_txWs.GetY();
		const Core::Vec4f& dxZAxis = m_txWs.GetZ();
		const Core::Vec4f& dxTAxis = m_txWs.GetT();
		DirectX::XMVECTOR xAxis = DirectX::XMVectorSet(dxXAxis.GetX(), dxXAxis.GetY(), dxXAxis.GetZ(), 0);
		DirectX::XMVECTOR yAxis = DirectX::XMVectorSet(dxYAxis.GetX(), dxYAxis.GetY(), dxYAxis.GetZ(), 0);
		DirectX::XMVECTOR zAxis = DirectX::XMVectorSet(dxZAxis.GetX(), dxZAxis.GetY(), dxZAxis.GetZ(), 0);
		DirectX::XMVECTOR tAxis = DirectX::XMVectorSet(dxTAxis.GetX(), dxTAxis.GetY(), dxTAxis.GetZ(), 1);
		DirectX::XMMATRIX dxTxWs(xAxis, yAxis, zAxis, tAxis);

		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

		//x axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(-DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 red(1, 0, 0, 0);

			DirectX::XMFLOAT4 appliedColor = red;
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kXAxis))
				appliedColor = m_hoverColor;

			RenderTranslationSingleAxis(rotation * dxTxWs, appliedColor);
		}

		//y axis
		{
			DirectX::XMFLOAT4 green(0, 1, 0, 0);
			DirectX::XMFLOAT4 appliedColor = green;
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kYAxis))
				appliedColor = m_hoverColor;

			RenderTranslationSingleAxis(dxTxWs, appliedColor);
		}

		//z axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(DirectX::XM_PIDIV2);

			DirectX::XMFLOAT4 blue(0, 0, 1, 0);
			DirectX::XMFLOAT4 appliedColor = blue;
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kZAxis))
				appliedColor = m_hoverColor;

			RenderTranslationSingleAxis(rotation * dxTxWs, appliedColor);
		}
	}

	void GizmoWidget::RenderScaleManipulator()
	{
		const Core::Vec4f& dxXAxis = m_txWs.GetX();
		const Core::Vec4f& dxYAxis = m_txWs.GetY();
		const Core::Vec4f& dxZAxis = m_txWs.GetZ();
		const Core::Vec4f& dxTAxis = m_txWs.GetT();
		DirectX::XMVECTOR xAxis = DirectX::XMVectorSet(dxXAxis.GetX(), dxXAxis.GetY(), dxXAxis.GetZ(), 0);
		DirectX::XMVECTOR yAxis = DirectX::XMVectorSet(dxYAxis.GetX(), dxYAxis.GetY(), dxYAxis.GetZ(), 0);
		DirectX::XMVECTOR zAxis = DirectX::XMVectorSet(dxZAxis.GetX(), dxZAxis.GetY(), dxZAxis.GetZ(), 0);
		DirectX::XMVECTOR tAxis = DirectX::XMVectorSet(dxTAxis.GetX(), dxTAxis.GetY(), dxTAxis.GetZ(), 1);
		DirectX::XMMATRIX dxTxWs(xAxis, yAxis, zAxis, tAxis);

		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

		//x axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(-DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 red(1, 0, 0, 0);
			RenderScaleSingleAxis(rotation * dxTxWs, red);
		}

		//y axis
		{
			DirectX::XMFLOAT4 green(0, 1, 0, 0);
			RenderScaleSingleAxis(dxTxWs, green);
		}

		//z axis
		{
			//rotate everything 90 degres around z axis
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(DirectX::XM_PIDIV2);
			DirectX::XMFLOAT4 blue(0, 0, 1, 0);
			RenderScaleSingleAxis(rotation * dxTxWs, blue);
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
			DirectX::XMVECTOR scale = DirectX::XMVectorSet(realLength, size, size, 0);
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

		Core::Vec4f xAxis = m_txWs.GetX();
		Core::Vec4f yAxis = m_txWs.GetY();
		Core::Vec4f zAxis = m_txWs.GetZ();
		Core::Vec4f tAxis(0, 0, 0, 1);

		xAxis.Normalize();
		yAxis.Normalize();
		zAxis.Normalize();

		Core::Mat44f txWs(xAxis, yAxis, zAxis, tAxis);

		m_eulerAngles = txWs.GetEulerAngle();
	}
}