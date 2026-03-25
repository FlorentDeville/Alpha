/********************************************************************************/
/* Copyright (C) 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Widgets/Viewport.h"

#include "Core/Collections/Array.h"
#include "Core/Guid/Guid.h"
#include "Core/Math/Mat44f.h"

#include "Rendering/ConstantBuffer/LightsCBuffer.h"

#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"
#include "Systems/Rendering/RenderPass/RenderView.h"

#include <map>

namespace Rendering
{
	class DescriptorHeap;
	class Mesh;
	class RenderTarget;
	class Texture;
}

namespace Systems
{
	class GameObject;
	class RenderableScene;
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

		Systems::RenderPassShadowMaps* m_pRenderPassShadowMaps;
		
		RenderPassObjectId* m_pRenderPassObjectId;
		Rendering::Texture* m_pReadbackBuffer;

		void Internal_Render() override;

		// Find the object id from the mouse position. The mouse position is local to the widget.
		uint32_t GetObjectId(int mouseX, int mouseY) const;

		void CreateRenderScene(Systems::RenderableScene& scene) const;

		void RenderView_LevelEditor(const Systems::RenderableScene& scene) const;

		float ComputeConstantScreenSizeScale(const Core::Vec4f& objectPosition) const;
	};
}
