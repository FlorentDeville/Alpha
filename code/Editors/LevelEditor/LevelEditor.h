/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Editors/LevelEditor/Level.h"

#include "Rendering/Material/MaterialId.h"
#include "Rendering/Mesh/MeshId.h"

#include "Systems/Assets/AssetId.h"

#include <map>
#include <DirectXMath.h>

namespace Rendering
{
	class RenderTarget;
}

namespace Widgets
{
	class Label;
	class SplitVertical;
	class Widget;
}

namespace Editors
{
	class BaseModel;
	class EntityModel;
	class EntityWidget;

	class LevelEditor : public Core::Singleton<LevelEditor>
	{
	public:
		LevelEditor();
		~LevelEditor();

		void CreateEditor(Widgets::Widget* pParent);

		void Update();
		void Render();

	private:
		Rendering::RenderTarget* m_pRenderTarget;
		float m_aspectRatio;

		char m_padding[4];

		//mouse controls
		DirectX::XMUINT2 m_mousePreviousPos;
		DirectX::XMMATRIX m_cameraTransform;
		bool m_firstFrameMouseDown;
		bool m_enableViewportControl;

		Level m_level;

		//entity viewer
		EntityModel* m_pEntityModel;
		EntityWidget* m_pEntityWidget;

		Widgets::Label* m_pEntityNameLabel;

		//temp
		std::map<Systems::AssetId, Rendering::MeshId> m_assetIdToMeshId;
		std::map<Systems::AssetId, Rendering::MaterialId> m_assetIdToMaterialId;

		void CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit);
		void CreateSceneTreeViewer(Widgets::SplitVertical* pSplit);

		bool OnTreeItemClicked(const BaseModel* pModel, int rowId);
	};
}