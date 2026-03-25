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
#include "Systems/Rendering/RenderPass/RenderPassId.h"

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

		Rendering::RootSignature* m_pObjectIdRootSig;
		Rendering::PipelineState* m_pObjectIdPso;
		Rendering::RenderTarget* m_pObjectIdRenderTarget; //the render target where the object ids are written
		Rendering::Texture* m_pReadbackBuffer;

		std::map<uint32_t, Core::Guid> m_objectIdToGuid;

		void Internal_Render() override;

		// Find the object id from the mouse position. The mouse position is local to the widget.
		uint32_t GetObjectId(int mouseX, int mouseY) const;

		void CreateRenderScene(Systems::RenderableScene& scene) const;

		void RenderView_LevelEditor(const Systems::RenderableScene& scene) const;
		void RenderView_ObjectId(const Systems::RenderableScene& scene);

		float ComputeConstantScreenSizeScale(const Core::Vec4f& objectPosition) const;
	};
}
