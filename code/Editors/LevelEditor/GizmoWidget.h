/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <DirectXMath.h>

#include "Core/Math/Ray.h"

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

		GizmoWidget();
		~GizmoWidget();

		void Update(const DirectX::XMVECTOR& mouse3dPosition);
		void Render();

		void SetModel(GizmoModel* pModel);

		void SetManipulatorMode(ManipulatorMode mode);

	private:
		void RenderRotationManipulator();
		void RenderTranslationManipulator();
		void RenderScaleManipulator();

		void RenderTranslationSingleAxis(const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color);
		void RenderScaleSingleAxis(const DirectX::XMMATRIX& txWs, const DirectX::XMFLOAT4& color) const;

		float ComputeConstantScreenSizeScale(const Core::Vec4f& objectPosition) const;

		void OnNodeChanged_Model(Node* pNode);

		GizmoModel* m_pModel;

		DirectX::XMMATRIX m_txWs;

		ManipulatorMode m_manipulatorMode;

		//standard size for translation gizmo
		const float BASE_DIAMETER;
		const float LENGTH;
		const float CONE_BASE_DIAMETER;
		const float CONE_BASE_LENGTH;

		//standard size for rotation gizmo
		const float ROTATION_DIAMATER;

		//standard size for scale gizmo
		const float SCALE_SQUARE_SIZE;

#if defined(DEBUG_RAY)
		Core::Ray m_debugRay;
#endif

#if defined(DEBUG_COLLISION)
		bool m_debugCollisionDetected;
#endif
	};
}
