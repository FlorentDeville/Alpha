/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Singleton.h"

namespace Widgets
{
	class TabContainer;
	class Widget;
}

namespace Editors
{
	class BaseEditor;
	class EditorParameter;

	enum class TabId
	{
		GAME_PLAYER = 0,
		LEVEL_EDITOR,
		MESH_EDITOR,
		MATERIAL_EDITOR,
		TEXTURE_EDITOR,
		LOG
	};

	class EditorManager : public Core::Singleton<EditorManager>
	{
	public:
		EditorManager();
		~EditorManager();

		void Init() override;
		void Init(const EditorParameter& param);
		void Shutdown() override;

		void SwitchTab(TabId id);

	private:
		Widgets::TabContainer* m_pTabContainer;

		Core::Array<BaseEditor*> m_editors;
	};
}

