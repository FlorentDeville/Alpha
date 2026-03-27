/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/LevelEditor/Widgets/GizmoWidget.h"

#include "Core/Math/Aabb.h"
#include "Core/Math/Constants.h"
#include "Core/Math/Intersection.h"
#include "Core/Math/Ray.h"
#include "Core/Math/Vec4f.h"
#include "Core/Math/Vectors.h"

#include "Editors/LevelEditor/LevelEditorModule.h"
#include "Editors/LevelEditor/Widgets/GizmoModel.h"

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
		, m_translationOffset()
		, m_hoverAxis()
		, m_hoverColor(0.99f, 0.49f, 0.22f, 1)
		, m_sqt()
		, m_enabled(true)
		, m_snapEnabled(false)
		, m_translationSnapDistance(0.1f)
		, m_rotationSnapDistance(45 * Core::PI_OVER_180)
		, m_scaleSnapDistance(2.f)
		, m_previousAngle(0)
		, m_previousScaleDistance(0)
	{}

	GizmoWidget::~GizmoWidget()
	{
		if (m_pModel)
		{
			delete m_pModel;
			m_pModel = nullptr;
		}
	}

	void GizmoWidget::Update(const Core::Vec4f& mouse3dPosition)
	{
		if (!m_enabled)
			return;

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
		if (!m_enabled || !m_pModel->ShouldRender())
			return;

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

		pModel->OnNodeChanged([this](const Core::Guid& nodeGuid) { OnNodeChanged_Model(); });
		m_sqt = pModel->GetTransform();
	}

	void GizmoWidget::SetManipulatorMode(ManipulatorMode mode)
	{
		m_manipulatorMode = mode;
	}

	void GizmoWidget::SetEnable(bool enable)
	{
		m_enabled = enable;
	}

	bool GizmoWidget::IsEnabled() const
	{
		return m_enabled;
	}

	void GizmoWidget::SetSnapping(bool enable)
	{
		m_snapEnabled = enable;
	}

	bool GizmoWidget::SnappingEnabled() const
	{
		return m_snapEnabled;
	}

	bool GizmoWidget::IsManipulating() const
	{
		return m_internalState == InternalState::kMoving;
	}

	bool GizmoWidget::IsHovering() const
	{
		return !m_hoverAxis.IsEmpty();
	}

	float GizmoWidget::GetSnapDistanceTranslation() const
	{
		return m_translationSnapDistance;
	}

	float GizmoWidget::GetSnapDistanceRotation() const
	{
		return m_rotationSnapDistance;
	}

	float GizmoWidget::GetSnapDistanceScale() const
	{
		return m_scaleSnapDistance;
	}

	void GizmoWidget::SetSnapDistanceTranslation(float distance)
	{
		m_translationSnapDistance = distance;
	}

	void GizmoWidget::SetSnapDistanceRotation(float distance)
	{
		m_rotationSnapDistance = distance;
	}

	void GizmoWidget::SetSnapDistanceScale(float distance)
	{
		m_scaleSnapDistance = distance;
	}

	void GizmoWidget::UpdateState_Idle(const Core::Vec4f& mouse3dPosition)
	{
		UpdateMouseHover(mouse3dPosition);

		if (!m_hoverAxis.IsEmpty())
		{
			if (Inputs::InputMgr::Get().IsMouseLeftButtonDown())
			{
				m_internalState = InternalState::kMoving;

				if (m_manipulatorMode == kTranslation)
				{
					//project the axis in screen space
					int axisIndex = m_hoverAxis.GetAxisIndex();
					
					const Core::Mat44f& txWs = m_sqt.GetMatrix();
					const Core::Vec4f& axis = txWs.GetRow(axisIndex);

					{
						const Core::Vec4f& A = LevelEditorModule::Get().GetCameraWs().GetT();
						Core::Vec4f aDir = mouse3dPosition - A;
						aDir.Normalize();

						Core::Vec4f B = txWs.GetT();
						Core::Vec4f bDir = axis;

						Core::Vec4f closestPointOnAxis;
						Core::Intersection::RayVsRay_ClosestPoint(Core::Ray(A, aDir), Core::Ray(B, bDir), closestPointOnAxis);
						
						m_translationOffset = closestPointOnAxis - B;
					}
				}
				else if (m_manipulatorMode == kRotation)
				{
					m_previousAngle = 0;
				}
				else if (m_manipulatorMode == kScale)
				{
					int axisIndex = m_hoverAxis.GetAxisIndex();

					const Core::Mat44f& txWs = m_sqt.GetMatrix();
					const Core::Vec4f& axis = txWs.GetRow(axisIndex);

					const Core::Vec4f& cameraRayOrigin = LevelEditorModule::Get().GetCameraWs().GetT();
					Core::Vec4f aDir = mouse3dPosition - cameraRayOrigin;
					aDir.Normalize();

					Core::Vec4f B = txWs.GetT();
					Core::Vec4f bDir = axis;

					Core::Vec4f closestPointOnAxis;
					Core::Intersection::RayVsRay_ClosestPoint(Core::Ray(cameraRayOrigin, aDir), Core::Ray(B, bDir), closestPointOnAxis);

					Core::Vec4f centerToMouse = closestPointOnAxis - B;
					m_previousScaleDistance = centerToMouse.Dot(axis);
				}
			}
		}
	}

	void GizmoWidget::UpdateState_Moving(const Core::Vec4f& mouse3dPosition)
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
		else if (m_manipulatorMode == kScale)
		{
			UpdateState_Moving_Scale(mouse3dPosition);
		}
	}

	void GizmoWidget::UpdateMouseHover(const Core::Vec4f& mouse3dPosition)
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
			UpdateMouseHoverScale(mouse3dPosition);
			break;

		default:
			break;
		}
	}

	void GizmoWidget::UpdateMouseHoverTranslation(const Core::Vec4f& mouse3dPosition)
	{
		//create ray 
		const Core::Mat44f& cameraWs = LevelEditorModule::Get().GetCameraWs();

		const Core::Vec4f& rayOrigin = cameraWs.GetT();
		Core::Vec4f rayDirection = mouse3dPosition - rayOrigin;
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
		const Core::Mat44f& txWs = m_sqt.GetMatrix();
		Core::Mat44f invTxWs = txWs.Inverse();

		Core::Ray mousePickingRayLs = mousePickingRay;
		mousePickingRayLs.Transform(invTxWs);

		//compute aabb size
		Core::Vec4f objectPosition = txWs.GetT();
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

	void GizmoWidget::UpdateMouseHoverRotation(const Core::Vec4f& mouse3dPosition)
	{
		//create ray 
		const Core::Mat44f& cameraWs = LevelEditorModule::Get().GetCameraWs();

		const Core::Vec4f& rayOrigin = cameraWs.GetT();
		Core::Vec4f rayDirection = mouse3dPosition - rayOrigin;
		rayDirection.Set(3, 0);
		rayDirection.Normalize();
		Core::Ray mousePickingRay(rayOrigin, rayDirection);

		//disk center
		const Core::Vec4f& diskCenter = m_sqt.GetTranslation();

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
			Core::Vec4f diskNormal = m_sqt.GetMatrix().GetRow(axisIndex);

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

	void GizmoWidget::UpdateMouseHoverScale(const Core::Vec4f& mouse3dPosition)
	{
		//create ray 
		const Core::Mat44f& cameraWs = LevelEditorModule::Get().GetCameraWs();

		const Core::Vec4f& rayOrigin = cameraWs.GetT();
		Core::Vec4f rayDirection = mouse3dPosition - rayOrigin;
		rayDirection.Set(3, 0);
		rayDirection.Normalize();
		Core::Ray mousePickingRay(rayOrigin, rayDirection);

		//convert ray to gizmo local space
		const Core::Mat44f& txWs = m_sqt.GetMatrix();
		Core::Mat44f invTxWs = txWs.Inverse();

		Core::Ray mousePickingRayLs = mousePickingRay;
		mousePickingRayLs.Transform(invTxWs);

		//compute aabb size
		Core::Vec4f objectPosition = txWs.GetT();
		float size = ComputeConstantScreenSizeScale(objectPosition);
		float realLength = LENGTH * size;
		const float BOX_HALF_SIZE = 0.5f * size;

		//check each axis
		for (int ii = 0; ii < 3; ++ii)
		{
			Core::Vec4f min(-BOX_HALF_SIZE, -BOX_HALF_SIZE, -BOX_HALF_SIZE, 0);
			min.Set(ii, 0);

			Core::Vec4f max(BOX_HALF_SIZE, BOX_HALF_SIZE, BOX_HALF_SIZE, 0);
			max.Set(ii, realLength);

			Core::Aabb axisAabb(min, max);

			//collision test, ray vs aabb
			bool collision = Core::Intersection::RayVsAabb(mousePickingRayLs, axisAabb);
			if (collision)
			{
				m_hoverAxis.SetAxisIndex(ii);
				return;
			}
		}
	}

	void GizmoWidget::UpdateState_Moving_Translation(const Core::Vec4f& mouse3dPosition)
	{
		//project the axis in screen space
		int axisIndex = m_hoverAxis.GetAxisIndex();

		const Core::Vec4f& axis = m_sqt.GetMatrix().GetRow(axisIndex);

		{
			const Core::Vec4f& A = LevelEditorModule::Get().GetCameraWs().GetT();
			Core::Vec4f aDir = mouse3dPosition - A;
			aDir.Normalize();

			const Core::Vec4f& B = m_sqt.GetTranslation();
			Core::Vec4f bDir = axis;

			Core::Vec4f closestPointOnAxis;
			Core::Intersection::RayVsRay_ClosestPoint(Core::Ray(A, aDir), Core::Ray(B, bDir), closestPointOnAxis);

			closestPointOnAxis = closestPointOnAxis - m_translationOffset;
			closestPointOnAxis.Set(3, 1);

			if (m_snapEnabled)
			{
				Core::Vec4f dt = closestPointOnAxis - m_sqt.GetTranslation();
				float distance = dt.Dot(bDir);
				int32_t incrementalCount = static_cast<int32_t>(distance / m_translationSnapDistance);
				closestPointOnAxis = m_sqt.GetTranslation() + bDir * m_translationSnapDistance * static_cast<float>(incrementalCount);
			}

			m_sqt.SetTranslation(closestPointOnAxis);
			m_pModel->Translate(closestPointOnAxis);
		}
	}

	void GizmoWidget::UpdateState_Moving_Rotation(const Core::Vec4f& mouse3dPosition)
	{
		//get the closest point from the ray to the disk
		const Core::Vec4f& rayOrigin = LevelEditorModule::Get().GetCameraWs().GetT();
		Core::Vec4f rayDir = mouse3dPosition - rayOrigin;
		rayDir.Normalize();

		//disk center
		Core::Vec4f diskCenter = m_sqt.GetTranslation();

		//disk axis
		int axisIndex = m_hoverAxis.GetAxisIndex();
		Core::Vec4f diskNormal = m_sqt.GetMatrix().GetRow(axisIndex);

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
		float angle = acosf(cos);
		if (angle == 0)
			return;

		if (sin < 0)
			angle = DirectX::XM_2PI - angle;
		
		//here angle is in radian, always positive between 0 and 2PI.

		float dtAngle = angle - m_previousAngle;
		if (dtAngle > DirectX::XM_PI)
			dtAngle = dtAngle - DirectX::XM_2PI;
		
		if(dtAngle < -DirectX::XM_PI)
			dtAngle = dtAngle + DirectX::XM_2PI;

		// here dtAngle is always the shortest angle. ie [0, PI] or [0, -PI]
		if (m_snapEnabled)
		{
			float absDtAngle = abs(dtAngle);
			if (absDtAngle < m_rotationSnapDistance)
			{
				return;
			}

			float sign = 1;
			if (dtAngle < 0)
				sign = -1;

			dtAngle = sign * m_rotationSnapDistance;
			angle = m_previousAngle + dtAngle;
		}

		Core::Vec4f rotationAxis(0, 0, 0, 1);
		rotationAxis.Set(axisIndex, 1);
		Core::Mat44f newRotation = Core::Mat44f::CreateRotationMatrix(rotationAxis, -dtAngle);
	
		m_previousAngle = angle;
		m_pModel->Rotate(newRotation);
		m_sqt = m_pModel->GetTransform();
	}

	void GizmoWidget::UpdateState_Moving_Scale(const Core::Vec4f& mouse3dPosition)
	{
		//project the axis in screen space
		int axisIndex = m_hoverAxis.GetAxisIndex();

		const Core::Vec4f& axis = m_sqt.GetMatrix().GetRow(axisIndex);

		{
			const Core::Vec4f& A = LevelEditorModule::Get().GetCameraWs().GetT();
			Core::Vec4f aDir = mouse3dPosition - A;
			aDir.Normalize();

			const Core::Vec4f& B = m_sqt.GetTranslation();
			Core::Vec4f bDir = axis;

			Core::Vec4f closestPointOnAxis;
			Core::Intersection::RayVsRay_ClosestPoint(Core::Ray(A, aDir), Core::Ray(B, bDir), closestPointOnAxis);

			Core::Vec4f centerToMouse = closestPointOnAxis - B;
			float distance = centerToMouse.Dot(axis);

			float dt = distance - m_previousScaleDistance;
			
			if (m_snapEnabled)
			{
				if (abs(dt) < m_scaleSnapDistance)
					return;

				int32_t incrementalCount = static_cast<int32_t>(dt / m_scaleSnapDistance);
				dt = incrementalCount * m_scaleSnapDistance;
			}

			Core::Vec4f scaleIncrement(0, 0, 0, 0);
			scaleIncrement.Set(axisIndex, dt);

			m_pModel->IncrementScale(scaleIncrement);
			m_previousScaleDistance = distance;
		}
	}

	void GizmoWidget::RenderRotationManipulator()
	{
		const Core::Mat44f& txWs = m_sqt.GetMatrix();

		Core::Vec4f objectPosition = txWs.GetT();
		float size = ComputeConstantScreenSizeScale(objectPosition);

		float realRotationDiameter = ROTATION_DIAMATER * size;
		Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(realRotationDiameter, realRotationDiameter, realRotationDiameter, 0));

		//rotation x axis
		{
			constexpr float rotationAngle = DirectX::XMConvertToRadians(90.f);
			Core::Mat44f rotation = Core::Mat44f::CreateRotationZ(rotationAngle);
			Core::Mat44f transform = scale * rotation * txWs;

			Core::Float4 red(1, 0, 0, 1);
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kXAxis))
				red = Core::Float4(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, m_hoverColor.w);

			Rendering::RenderModule::Get().RenderPrimitiveTorus(transform.m_matrix, red);
		}

		//rotation y axis
		{
			Core::Float4 green(0, 1, 0, 1);
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kYAxis))
				green = Core::Float4(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, m_hoverColor.w);

			Core::Mat44f transform = scale * txWs;
			Rendering::RenderModule::Get().RenderPrimitiveTorus(transform.m_matrix, green);
		}

		//rotation z axis
		{
			constexpr float rotationAngle = DirectX::XMConvertToRadians(90.f);
			Core::Mat44f rotation = Core::Mat44f::CreateRotationX(rotationAngle);
			Core::Mat44f transform = scale * rotation * txWs;

			Core::Float4 blue(0, 0, 1, 1);
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kZAxis))
				blue = Core::Float4(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, m_hoverColor.w);

			Rendering::RenderModule::Get().RenderPrimitiveTorus(transform.m_matrix, blue);
		}
	}

	void GizmoWidget::RenderTranslationManipulator()
	{
		const Core::Mat44f& txWs = m_sqt.GetMatrix();

		//x axis
		{
			//rotate everything 90 degres around z axis
			Core::Mat44f rotation = Core::Mat44f::CreateRotationZ(-Core::PI_OVER_TWO);
			Core::Mat44f transform = rotation * txWs;

			Core::Float4 red(1, 0, 0, 0);
			Core::Float4 appliedColor = red;
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kXAxis))
				appliedColor = Core::Float4(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, m_hoverColor.w);

			RenderTranslationSingleAxis(transform, appliedColor);
		}

		//y axis
		{
			Core::Float4 green(0, 1, 0, 0);
			Core::Float4 appliedColor = green;
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kYAxis))
				appliedColor = Core::Float4(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, m_hoverColor.w);

			RenderTranslationSingleAxis(txWs, appliedColor);
		}

		//z axis
		{
			//rotate everything 90 degres around z axis
			Core::Mat44f rotation = Core::Mat44f::CreateRotationX(Core::PI_OVER_TWO);
			Core::Mat44f transform = rotation * txWs;

			Core::Float4 blue(0, 0, 1, 0);
			Core::Float4 appliedColor = blue;
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kZAxis))
				appliedColor = Core::Float4(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, m_hoverColor.w);

			RenderTranslationSingleAxis(transform, appliedColor);
		}
	}

	void GizmoWidget::RenderScaleManipulator()
	{
		const Core::Mat44f& txWs = m_sqt.GetMatrix();

		//x axis
		{
			//rotate everything 90 degres around z axis
			Core::Mat44f rotation = Core::Mat44f::CreateRotationZ(-Core::PI_OVER_TWO);
			Core::Mat44f transform = rotation * txWs;

			Core::Float4 red(1, 0, 0, 0);
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kXAxis))
				red = Core::Float4(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, m_hoverColor.w);

			RenderScaleSingleAxis(transform, red);
		}

		//y axis
		{
			Core::Float4 green(0, 1, 0, 0);
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kYAxis))
				green = Core::Float4(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, m_hoverColor.w);

			RenderScaleSingleAxis(txWs, green);
		}

		//z axis
		{
			//rotate everything 90 degres around z axis
			Core::Mat44f rotation = Core::Mat44f::CreateRotationX(Core::PI_OVER_TWO);
			Core::Mat44f transform = rotation * txWs;

			Core::Float4 blue(0, 0, 1, 0);
			if (m_hoverAxis.Contains(GizmoAxis::GizmoAxisEnum::kZAxis))
				blue = Core::Float4(m_hoverColor.x, m_hoverColor.y, m_hoverColor.z, m_hoverColor.w);

			RenderScaleSingleAxis(transform, blue);
		}
	}

	void GizmoWidget::RenderTranslationSingleAxis(const Core::Mat44f& txWs, const Core::Float4& color)
	{
		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();
		
		Core::Vec4f objectPosition = txWs.GetT();
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
			Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(realDiameter, realLength, realDiameter, 0));
			Core::Mat44f translation = Core::Mat44f::CreateTranslationMatrix(Core::Vec4f(0.f, realLength * 0.5f, 0, 0));
			Core::Mat44f txLs = scale * translation;
			Core::Mat44f ws = txLs * txWs;
			renderingMgr.RenderPrimitiveCylinder(ws.m_matrix, color);
		}

		{		
			float coneDiameter = CONE_BASE_DIAMETER * size;
			float coneLength = CONE_BASE_LENGTH * size;

			Core::Mat44f localScale = Core::Mat44f::CreateScaleMatrix(Core::Vec4f(coneDiameter, coneLength, coneDiameter, 0));
			Core::Mat44f localTranslation = Core::Mat44f::CreateTranslationMatrix(Core::Vec4f(0.f, realLength, 0, 0));
			Core::Mat44f txLs = localScale * localTranslation;
			Core::Mat44f ws = txLs * txWs;
			renderingMgr.RenderPrimitiveCone(ws.m_matrix, color);
		}
	}

	void GizmoWidget::RenderScaleSingleAxis(const Core::Mat44f& txWs, const Core::Float4& color) const
	{
		Rendering::RenderModule& renderingMgr = Rendering::RenderModule::Get();

		Core::Vec4f objectPosition = txWs.GetT();
		float size = ComputeConstantScreenSizeScale(objectPosition);

		float realDiameter = BASE_DIAMETER * size;
		float realLength = LENGTH * size;

		{
			Core::Mat44f scale = Core::Mat44f::CreateScaleMatrix(realDiameter, realLength, realDiameter);
			Core::Mat44f translation = Core::Mat44f::CreateTranslationMatrix(0.f, realLength * 0.5f, 0);
			Core::Mat44f ws = scale * translation * txWs;
			renderingMgr.RenderPrimitiveCylinder(ws.m_matrix, color);
		}

		{
			float coneDiameter = CONE_BASE_DIAMETER * size;
			float coneLength = SCALE_SQUARE_SIZE * size;

			Core::Mat44f localScale = Core::Mat44f::CreateScaleMatrix(coneLength);
			Core::Mat44f localTranslation = Core::Mat44f::CreateTranslationMatrix(0, realLength, 0);
			Core::Mat44f ws = localScale * localTranslation * txWs;
			renderingMgr.RenderPrimitiveCube(ws.m_matrix, color);
		}
	}

	float GizmoWidget::ComputeConstantScreenSizeScale(const Core::Vec4f& objectPosition) const
	{
		const LevelEditorModule& levelEditor = LevelEditorModule::Get();
		const Core::Mat44f& camera = levelEditor.GetCameraWs();
		float fov = levelEditor.GetFovRad();

		Core::Vec4f dt = camera.GetT() - objectPosition;
		float distance = dt.Length();
		float worldSize = (2 * tanf(fov * 0.5f)) * distance;
		const float SCREEN_RATIO = 0.025f;
		float size = SCREEN_RATIO * worldSize;
		return size;
	}

	void GizmoWidget::OnNodeChanged_Model()
	{
		m_sqt = m_pModel->GetTransform();
	}
}