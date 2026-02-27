/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/BaseEditor.h"

#include <vector>

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

		void CreateEditor(Widgets::Widget* pParent) override;

	private:
		TextureListModel* m_pListModel;

		PropertyGridPopulator* m_pPopulator;

		float m_renderTargetHalfWidth;
		float m_renderTargetHalfHeight;

		float m_scale;

		void OnClick_Texture_Import();
		void OnClick_Cubemap_NewCubemap();
		void OnClick_File_Delete();
		void OnClick_File_Rename();

		Systems::NewAssetId GetSelectedTextureId() const;

		void Viewport_OnRender();
		void Viewport_OnUpdate(uint64_t dt);

		void OnSelectionChanged(const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected);
	};
}
