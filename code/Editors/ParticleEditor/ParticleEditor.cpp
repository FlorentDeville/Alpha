/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/ParticleEditor/ParticleEditor.h"

#include "Editors/EditorParameter.h"

namespace Editors
{
	ParticleEditor::ParticleEditor()
		: BaseEditor()
	{ }

	ParticleEditor::~ParticleEditor()
	{ }

	void ParticleEditor::CreateEditor(const EditorParameter& param)
	{
		CreateDefaultWidgets(param.m_pParent, "Particles");
	}
}
