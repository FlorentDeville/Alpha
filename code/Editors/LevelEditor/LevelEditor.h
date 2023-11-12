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
	class GizmoWidget;
	class LevelTreeModel;
	class Node;
	class TreeWidget;

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
		bool m_firstFrameMouseDown;
		bool m_enableViewportControl;

		char m_padding2[14];

		//camera transform
		DirectX::XMMATRIX m_cameraRotation;		//matrix for the orientation of the camera (same as m_cameraEulerAngle)
		DirectX::XMVECTOR m_cameraEulerAngle;	//euler angle of the camera (sme as m_cameraRotation)
		DirectX::XMVECTOR m_cameraPosition;		//position of the camera in world space
		DirectX::XMMATRIX m_cameraTransform;	//transform of the camera in world space

		Level m_level;

		//entity viewer
		EntityModel* m_pEntityModel;
		EntityWidget* m_pEntityWidget;
		TreeWidget* m_pTreeWidget;
		LevelTreeModel* m_pLevelTreeModel;

		Widgets::Label* m_pEntityNameLabel;

		bool m_isPanning; //when the user is panning the camera

		GizmoWidget* m_pGizmoWidget;

		//temp
		std::map<Systems::AssetId, Rendering::MeshId> m_assetIdToMeshId;
		std::map<Systems::AssetId, Rendering::MaterialId> m_assetIdToMaterialId;

		void CreateEntityPropertyGrid(Widgets::SplitVertical* pSplit);
		void CreateSceneTreeViewer(Widgets::SplitVertical* pSplit);

		bool OnTreeItemClicked(BaseModel* pModel, int rowId);
		bool OnAddEntityClicked();
		bool OnRemoveEntityClicked();

		void AddNewEntity(const std::string& name);
		void DeleteEntity(Node* pNode);
	};
}