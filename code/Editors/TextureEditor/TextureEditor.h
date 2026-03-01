/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/BaseEditor.h"
#include "Editors/ObjectWatcher/ObjectWatcherCallbackId.h"

#include <vector>

namespace Rendering
{
	class Mesh;
	class PipelineState;
	class RootSignature;
	class Shader;
}

namespace Systems
{
	class NewAssetId;
}

namespace Widgets
{
	class SelectionRow;
	class Widget;
}

namespace Editors
{
	class PropertyGridPopulator;
	class TextureListModel;

	class TextureEditor : public BaseEditor
	{
	public:
		TextureEditor();
		~TextureEditor();

		void CreateEditor(const EditorParameter& param) override;

	private:
		TextureListModel* m_pListModel;

		PropertyGridPopulator* m_pPopulator;

		float m_renderTargetHalfWidth;
		float m_renderTargetHalfHeight;

		float m_cameraDistance;

		ObjectWatcherCallbackId m_objWatcherCid;
		
		Rendering::Mesh* m_pQuad;
		Rendering::Mesh* m_pCube;

		Rendering::Shader* m_pTextureViewportVertexShader;
		Rendering::Shader* m_pTextureViewportPixelShader;
		Rendering::RootSignature* m_pRootSig;

		Rendering::PipelineState* m_pPsoQuad;
		Rendering::PipelineState* m_pPsoCubemap;

		float m_aspectRatio;

		void OnClick_Texture_CreateAndImport();
		void OnClick_Texture_Import();
		
		void OnClick_Cubemap_CreateCubemap();
		void OnClick_Cubemap_Import();

		void OnClick_File_Save();
		void OnClick_File_Delete();
		void OnClick_File_Rename();

		Systems::NewAssetId GetSelectedTextureId() const;

		void Viewport_OnRender();
		void Viewport_OnUpdate(uint64_t dt);

		void OnSelectionChanged(const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected);
		void OnDataChanged();
	};
}
