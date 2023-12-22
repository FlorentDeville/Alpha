/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Viewport_v2.h"

namespace Editors
{
	class Camera;
	class GizmoModel;
	class GizmoWidget;

	class LevelEditorViewportWidget : public Widgets::Viewport_v2
	{
	public:
		LevelEditorViewportWidget(int width, int height);
		~LevelEditorViewportWidget();

		void Render() override;
		void Update(uint64_t dt) override;

		void SetEnableViewportControl(bool enable);

		GizmoWidget* GetGizmoWidget();
		GizmoModel* GetGizmoModel();

	private:
		GizmoWidget* m_pGizmoWidget;
		GizmoModel* m_pGizmoModel;

		Camera* m_pCamera;
		
		bool m_enableViewportControl;
		
		bool m_isPanning; //when the user is panning the camera		
	};
}
