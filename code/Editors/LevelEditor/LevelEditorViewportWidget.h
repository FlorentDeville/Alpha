/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Viewport_v2.h"

#include <DirectXMath.h>

namespace Editors
{
	class LevelEditorViewportWidget : public Widgets::Viewport_v2
	{
	public:
		LevelEditorViewportWidget(int width, int height);
		~LevelEditorViewportWidget();

		void Render() override;
		void Update(uint64_t dt) override;

		void SetEnableViewportControl(bool enable);

	private:

		//camera transform
		DirectX::XMMATRIX m_cameraRotation;		//matrix for the orientation of the camera (same as m_cameraEulerAngle)
		DirectX::XMVECTOR m_cameraEulerAngle;	//euler angle of the camera (sme as m_cameraRotation)
		DirectX::XMVECTOR m_cameraPosition;		//position of the camera in world space
		DirectX::XMMATRIX m_cameraTransform;	//transform of the camera in world space

		bool m_enableViewportControl;
		bool m_firstFrameMouseDown;
		bool m_isPanning; //when the user is panning the camera
		DirectX::XMUINT2 m_mousePreviousPos;
	};
}
