/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"

#include "Editors/BaseEditor.h"
#include "Editors/ObjectWatcher/ObjectWatcherCallbackId.h"

#include "Systems/Particle/ParticleSystem.h"

#include <vector>

namespace Rendering
{
	class PipelineState;
	class RootSignature;
}

namespace Systems
{
	class RenderPassBase;
}

namespace Widgets
{
	class SelectionRow;
	class Viewport;
}

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

		Core::Array<ObjectWatcherCallbackId> m_objWatcherCid;

		Systems::ParticleSystem m_particleSystem;

		Systems::RenderPassBase* m_pBasePass;

		Widgets::Viewport* m_pViewport;

		Rendering::RootSignature* m_pCopyRootSig;
		Rendering::PipelineState* m_pCopyPso;

		void OnMenu_File_Create();
		void OnMenu_File_Delete();
		void OnMenu_File_Save();
		void OnMenu_File_Rename();

		void OnSelectionChanged(const std::vector<Widgets::SelectionRow>& selected, const std::vector<Widgets::SelectionRow>& deselected);

		void OnParticleEffectModified();

		void Viewport_OnUpdate(uint64_t dt);
		void Viewport_OnRender();

		void RemoveAllWatchers();
	};
}
