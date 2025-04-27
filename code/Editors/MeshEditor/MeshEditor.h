/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Editors/RawDb.h"

#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/Material/MaterialId.h"
#include "Rendering/Texture/TextureId.h"

#include "Systems/Assets/AssetId.h"
#include "Systems/Assets/NewAssetId.h"

#include "DirectXMath.h"

#include <map>
#include <vector>
#include <string>

namespace Rendering
{
	class RenderTarget;
}

namespace Systems
{
	class MeshAsset_v2;
}

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
		Systems::MeshAsset_v2* m_mesh;
		
		MeshEntry();
	};

	class MaterialEntry
	{
	public:
		std::string m_name;
		Rendering::MaterialId m_materialId;

		MaterialEntry();
	};

	class MeshEditorParameter
	{
	public:
		Widgets::Widget* pParent;
		std::string m_dataMeshPath;
		std::string m_dataMaterialPath;
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

		void Update();
		void Render();

	private:
		//camera position
		float m_cameraDistance;
		float m_aspectRatio;
		DirectX::XMVECTOR m_cameraTarget;
		DirectX::XMVECTOR m_cameraEuler;
		
		Rendering::RenderTarget* m_pRenderTarget;

		std::vector<MeshEntry> m_allMeshes;
		std::vector<MaterialEntry> m_allMaterials;

		int m_selectedMesh; //id in m_allMeshes, -1 if nothing selected

		Rendering::MaterialId m_materialId; //material to use to render the mesh

		Rendering::TextureId m_importIconTextureId;

		bool m_enableViewportControl;
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
	};
}
