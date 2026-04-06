/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Viewport.h"

namespace Rendering
{
	class DescriptorHeap;
	class Mesh;
	class PipelineState;
	class RenderTarget;
	class RootSignature;
	class Texture;
}

namespace Systems
{
	class GameObject;
	class RenderableScene;
	class RenderPassBase;
	class RenderPassBloom;
	class RenderPassShadowMaps;
}

namespace Editors
{
	class CameraWidget;
	class GizmoModel;
	class GizmoWidget;
	class RenderPassObjectId;

	class LevelEditorViewportWidget : public Widgets::Viewport
	{
	public:
		LevelEditorViewportWidget(int width, int height);
		~LevelEditorViewportWidget();

		void Update(uint64_t dt) override;

		bool Handle(const Widgets::GlobalEvent& event) override;

		void SetEnableViewportControl(bool enable);

		GizmoWidget* GetGizmoWidget();
		GizmoModel* GetGizmoModel();

	private:
		GizmoWidget* m_pGizmoWidget;
		GizmoModel* m_pGizmoModel;

		CameraWidget* m_pCamera;
		
		bool m_enableViewportControl;
		
		bool m_isPanning; //when the user is panning the camera	

		Systems::RenderPassBase* m_pRenderPassBase;
		Systems::RenderPassShadowMaps* m_pRenderPassShadowMaps;
		Systems::RenderPassBloom* m_pRenderPassBloom;

		RenderPassObjectId* m_pRenderPassObjectId;
		Rendering::Texture* m_pReadbackBuffer;

		Rendering::RootSignature* m_pCopyRootSig;
		Rendering::PipelineState* m_pCopyPso;

		void Internal_Render() override;

		// Find the object id from the mouse position. The mouse position is local to the widget.
		uint32_t GetObjectId(int mouseX, int mouseY) const;
	};
}
