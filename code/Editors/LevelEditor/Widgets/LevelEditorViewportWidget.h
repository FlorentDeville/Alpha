/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Viewport_v2.h"

#include "Core/Guid/Guid.h"

#include <map>

namespace Rendering
{
	class RenderTarget;
	class Texture;
}

namespace Editors
{
	class CameraWidget;
	class GizmoModel;
	class GizmoWidget;

	class LevelEditorViewportWidget : public Widgets::Viewport_v2
	{
	public:
		LevelEditorViewportWidget(int width, int height);
		~LevelEditorViewportWidget();

		void Render();
		void Update(uint64_t dt) override;

		void SetEnableViewportControl(bool enable);

		GizmoWidget* GetGizmoWidget();
		GizmoModel* GetGizmoModel();

	private:
		GizmoWidget* m_pGizmoWidget;
		GizmoModel* m_pGizmoModel;

		CameraWidget* m_pCamera;
		
		bool m_enableViewportControl;
		
		bool m_isPanning; //when the user is panning the camera	

		Rendering::RenderTarget* m_pObjectIdRenderTarget; //the render target where the object ids are written
		Rendering::Texture* m_pReadbackBuffer;

		std::map<uint32_t, Core::Guid> m_objectIdToGuid;

		void Internal_Render() override;
	};
}
