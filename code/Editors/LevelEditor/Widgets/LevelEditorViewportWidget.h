/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Widgets/Viewport_v2.h"

#include "Core/Collections/Array.h"
#include "Core/Guid/Guid.h"
#include "Core/Math/Mat44f.h"

#include "Rendering/ConstantBuffer/LightsCBuffer.h"

#include "Systems/Assets/AssetObjects/MaterialInstance/MaterialInstanceAsset.h"

#include <map>

namespace Rendering
{
	class Mesh;
	class RenderTarget;
	class Texture;
}

namespace Systems
{
	class GameObject;
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

		void Update(uint64_t dt) override;

		bool Handle(const Widgets::BaseEvent& event) override;

		void SetEnableViewportControl(bool enable);

		GizmoWidget* GetGizmoWidget();
		GizmoModel* GetGizmoModel();

	private:
		
		struct Renderable
		{
			const Rendering::Mesh* m_pMesh;
			const Systems::MaterialInstanceAsset* m_pMaterial;
			const Systems::GameObject* m_pOwner;
			Core::Mat44f m_worldTx;
			
			bool m_primitiveMesh; // Call RenderBaseShape and don't use m_pMaterial
		};

		struct Light
		{
			Core::Mat44f m_lightSpaceTX; // transform from world space to light space.
			Rendering::LightCBuffer m_cbuffer;
		};

		GizmoWidget* m_pGizmoWidget;
		GizmoModel* m_pGizmoModel;

		CameraWidget* m_pCamera;
		
		bool m_enableViewportControl;
		
		bool m_isPanning; //when the user is panning the camera	

		Rendering::RenderTarget* m_pShadowRenderTarget;

		Rendering::RenderTarget* m_pObjectIdRenderTarget; //the render target where the object ids are written
		Rendering::Texture* m_pReadbackBuffer;

		std::map<uint32_t, Core::Guid> m_objectIdToGuid;

		void Internal_Render() override;

		// Find the object id from the mouse position. The mouse position is local to the widget.
		uint32_t GetObjectId(int mouseX, int mouseY) const;

		void CreateRenderScene(Core::Array<Renderable>& renderables, Core::Array<Light>& lights) const;

		void RenderView_LevelEditor(Core::Array<Renderable>& renderables, Core::Array<Light>& lights) const;
		void RenderView_ObjectId(Core::Array<Renderable>& renderables);
		void RenderView_ShadowMap(Core::Array<Renderable>& renderables, Core::Array<Light>& lights) const;

		float ComputeConstantScreenSizeScale(const Core::Vec4f& objectPosition) const;
	};
}
