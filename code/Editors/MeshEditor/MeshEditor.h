/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/BaseEditor.h"

#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/Texture/TextureId.h"

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
#include "Systems/Assets/NewAssetId.h"

#include "DirectXMath.h"

#include <map>
#include <vector>
#include <string>

namespace Widgets
{
	class Button;
	class Layout;
	class SelectionRow;
	class Text;
	class Widget;
}

namespace Editors
{
	class MeshListModel;

	class MaterialEntry
	{
	public:
		Systems::NewAssetId m_Id;

		MaterialEntry();
	};

	class MeshEditor : public BaseEditor
	{
	public:
		MeshEditor();
		~MeshEditor();

		void CreateEditor(Widgets::Widget* pParent) override;

	private:
		//camera position
		DirectX::XMVECTOR m_cameraTarget;
		DirectX::XMVECTOR m_cameraEuler;

		float m_cameraDistance;
		float m_aspectRatio;
				
		std::vector<MaterialEntry> m_allMaterials;

		Systems::NewAssetId m_materialId; //material to use to render the mesh

		Rendering::TextureId m_importIconTextureId;

		DirectX::XMUINT2 m_mousePreviousPos;
		bool m_firstFrameMouseDown;

		std::vector<Widgets::Button*> m_allEntryButton;
		
		Widgets::Text* m_pLogWidget;

		MeshListModel* m_pMeshListModel;

		Systems::MeshAsset* m_pSelectedMesh;

		bool OnMeshImportClicked(int entryIndex);
		bool OnMaterialClicked(int entryIndex);

		bool OnSaveSelectedMeshClicked();

		void Viewport_OnUpdate();
		void Viewport_OnRender();

		void MeshTableView_OnSelectionChanged(const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected);
	};
}
