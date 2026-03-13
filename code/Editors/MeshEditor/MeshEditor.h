/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/BaseEditor.h"

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"
#include "Core/Math/Vectors.h"

#include "Systems/Assets/NewAssetId.h"

#include <vector>

namespace Rendering
{
	class RenderTarget;
}

namespace Systems
{
	class MeshAsset;
}

namespace Widgets
{
	class Button;
	class Icon;
	class SelectionRow;
}

namespace Editors
{
	class MeshListModel;
	class PropertyGridPopulator;

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

		void CreateEditor(const EditorParameter& param) override;

	private:
		//camera position
		Core::Vec4f m_cameraTarget;
		Core::Vec4f m_cameraEuler;
		Core::Vec4f m_cameraPosition;
		Core::Mat44f m_cameraView;

		float m_cameraDistance;
		float m_aspectRatio;
				
		std::vector<MaterialEntry> m_allMaterials;

		Systems::NewAssetId m_materialId; //material to use to render the mesh

		Rendering::RenderTarget* m_pWorldAxisRenderTarget;
		float m_pWorldAxisRTRatio;

		Core::Int2 m_mousePreviousPos;
		bool m_firstFrameMouseDown;

		std::vector<Widgets::Button*> m_allEntryButton;
		
		MeshListModel* m_pMeshListModel;

		Systems::MeshAsset* m_pSelectedMesh;

		PropertyGridPopulator* m_pPopulator;

		bool OnMaterialClicked(int entryIndex);

		void OnClicked_File_Save();
		void OnClicked_File_Import();
		void OnClicked_File_Reimport();
		void OnClicked_File_Rename();
		void OnClicked_File_Delete();

		void Viewport_OnUpdate();
		void Viewport_OnRender();
		void Viewport_OnPreRender();

		void MeshTableView_OnSelectionChanged(const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected);

		void ComputeCameraPositionAndView();
	};
}
