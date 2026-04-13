/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/BaseEditor.h"

namespace Editors
{
	class PropertyGridPopulator;
	class ParticleListModel;

	class ParticleEditor : public BaseEditor
	{
		using ParentClass = BaseEditor;

	public:
		ParticleEditor();
		~ParticleEditor();

		void CreateEditor(const EditorParameter& param) override;

	private:
		PropertyGridPopulator* m_pPopulator;

		ParticleListModel* m_pListModel;

		void OnMenu_File_Create();
		void OnMenu_File_Delete();
	};
}
