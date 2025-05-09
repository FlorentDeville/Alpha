/********************************************************************/
/* � 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

#include "Core/Math/Sqt.h"

#include "Editors/LevelEditor/Widgets/GizmoAxis.h"

#if defined(_DEBUG)
//#define DEBUG_RAY
//#define DEBUG_COLLISION
#endif

namespace Core
{
	class Vec4f;
}

namespace Editors
{
	class GizmoModel;
	class Node;

	class GizmoWidget
	{
	public:
		enum ManipulatorMode
		{
			kTranslation,
			kRotation,
			kScale,

			kCount
		};

		enum class InternalState
		{
			kUnknown = -1,
			kIdle,
			kMoving,

			kCount
		};

		GizmoWidget();
		~GizmoWidget();

		void Update(const Core::Vec4f& mouse3dPosition);
		void Render();

		void SetModel(GizmoModel* pModel);

		void SetManipulatorMode(ManipulatorMode mode);

		void SetEnable(bool enable);

		bool IsEnabled() const;

		void SetSnapping(bool enable);
		bool SnappingEnabled() const;

	private:
		void UpdateState_Idle(const Core::Vec4f& mouse3dPosition);
		void UpdateState_Moving(const Core::Vec4f& mouse3dPosition);

		void UpdateMouseHover(const Core::Vec4f& mouse3dPosition);
		void UpdateMouseHoverTranslation(const Core::Vec4f& mouse3dPosition);
		void UpdateMouseHoverRotation(const Core::Vec4f& mouse3dPosition);
		void UpdateMouseHoverScale(const Core::Vec4f& mouse3dPosition);

		void UpdateState_Moving_Translation(const Core::Vec4f& mouse3dPosition);
		void UpdateState_Moving_Rotation(const Core::Vec4f& mouse3dPosition);
		void UpdateState_Moving_Scale(const Core::Vec4f& mouse3dPosition);

		void RenderRotationManipulator();
		void RenderTranslationManipulator();
		void RenderScaleManipulator();

		void RenderTranslationSingleAxis(const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color);
		void RenderScaleSingleAxis(const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color) const;

		float ComputeConstantScreenSizeScale(const Core::Vec4f& objectPosition) const;

		void OnNodeChanged_Model();

		bool m_enabled;

		InternalState m_internalState;

		GizmoModel* m_pModel;

		Core::Sqt m_sqt;

		ManipulatorMode m_manipulatorMode;

		DirectX::XMUINT2 m_previousMousePosition;

		Core::Vec4f m_translationOffset;
		Core::Vec4f m_rotationInitialPoint; //where the user clicked for the first time
		float m_previousAngle;
		float m_previousScaleDistance;

		//standard size for translation gizmo
		const float BASE_DIAMETER;
		const float LENGTH;
		const float CONE_BASE_DIAMETER;
		const float CONE_BASE_LENGTH;

		//standard size for rotation gizmo
		const float ROTATION_DIAMATER;

		//standard size for scale gizmo
		const float SCALE_SQUARE_SIZE;

		GizmoAxis m_hoverAxis;
		DirectX::XMFLOAT4 m_hoverColor;

		bool m_snapEnabled;
		float m_translationSnapDistance;
		float m_rotationSnapDistance;
		float m_scaleSnapDistance;

#if defined(DEBUG_RAY)
		Core::Ray m_debugRay;
#endif

#if defined(DEBUG_COLLISION)
		bool m_debugCollisionDetected;
#endif
	};
}
