/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Editors/BaseEditor.h"

namespace Editors
{
	class ParticleEditor : public BaseEditor
	{
		using ParentClass = BaseEditor;

	public:
		ParticleEditor();
		~ParticleEditor();

		void CreateEditor(const EditorParameter& param) override;
	};
}
