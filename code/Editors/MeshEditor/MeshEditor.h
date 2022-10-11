/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Rendering/Mesh/MeshId.h"
#include "Rendering/PipelineState/PipelineState.h"
#include "Rendering/Material/MaterialId.h"

#include "DirectXMath.h"

#include <vector>
#include <string>

class Widget;
namespace Rendering
{
	class RenderTarget;
}

namespace Widgets
{
	class Button;
	class Layout;
	class Text;
}

namespace Editors
{
	class MeshEntry
	{
	public:
		std::string m_rawFilename;		//blender filename
		std::string m_binFilename;		//binary filename
		std::string m_displayName;		//name of the mesh in the editor
		Rendering::MeshId m_meshId;
		
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
		Widget* pParent;
		std::string m_dataMeshPath;
		std::string m_dataMaterialPath;
		std::string m_rawBlenderPath;
		std::string m_editorIconsPath;
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

		bool m_enableViewportControl;
		DirectX::XMUINT2 m_mousePreviousPos;
		bool m_firstFrameMouseDown;

		std::vector<Widgets::Button*> m_allEntryButton;
		
		Widgets::Text* m_pLogWidget;

		void ShowMesh(int entryIndex);
		void LoadMesh(MeshEntry& entry);

		void OnMeshEntryClicked(int entryIndex);
		bool OnMeshImportClicked(int entryIndex);
		bool OnMaterialClicked(int entryIndex);
	};
}
