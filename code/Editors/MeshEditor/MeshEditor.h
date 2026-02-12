/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/Material/MaterialId.h"
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
	class Text;
	class Widget;
}

namespace Editors
{
	class MeshEntry
	{
	public:
		std::string m_displayName;		//name of the mesh in the editor
		Systems::NewAssetId m_id;
		Systems::MeshAsset* m_mesh;
		
		MeshEntry();
	};

	class MaterialEntry
	{
	public:
		Systems::NewAssetId m_Id;

		MaterialEntry();
	};

	class MeshEditorParameter
	{
	public:
		Widgets::Widget* pParent;
		std::string m_rawBlenderPath;
		std::string m_editorScriptsPath;
		std::string m_blender;
	};

	class MeshEditor : public Core::Singleton<MeshEditor>
	{
	public:
		MeshEditor();
		~MeshEditor();

		void CreateEditor(const MeshEditorParameter& parameter);

	private:
		//camera position
		float m_cameraDistance;
		float m_aspectRatio;
		DirectX::XMVECTOR m_cameraTarget;
		DirectX::XMVECTOR m_cameraEuler;
		
		std::vector<MeshEntry> m_allMeshes;
		std::vector<MaterialEntry> m_allMaterials;

		int m_selectedMesh; //id in m_allMeshes, -1 if nothing selected

		Systems::NewAssetId m_materialId; //material to use to render the mesh

		Rendering::TextureId m_importIconTextureId;

		DirectX::XMUINT2 m_mousePreviousPos;
		bool m_firstFrameMouseDown;

		std::vector<Widgets::Button*> m_allEntryButton;
		
		Widgets::Text* m_pLogWidget;

		std::string m_editorScriptsPath;
		std::string m_blender;

		void ShowMesh(int entryIndex);
		void LoadMesh(MeshEntry& entry);

		void OnMeshEntryClicked(int entryIndex);
		bool OnMeshImportClicked(int entryIndex);
		bool OnMaterialClicked(int entryIndex);

		bool OnSaveSelectedMeshClicked();

		void Viewport_OnUpdate();
		void Viewport_OnRender();
	};
}
