/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/EditorManager.h"

#include "Editors/EditorParameter.h"
#include "Editors/GamePlayer/GamePlayer.h"
#include "Editors/LevelEditor/Widgets/LevelEditor.h"
#include "Editors/MaterialEditor/MaterialEditor.h"
#include "Editors/MeshEditor/MeshEditor.h"
#include "Editors/TextureEditor/TextureEditor.h"

#include "Widgets/TabContainer.h"

namespace Editors
{
	EditorManager::EditorManager()
		: Singleton<EditorManager>()
		, m_editors()
	{ }

	EditorManager::~EditorManager()
	{ }

	void EditorManager::Init()
	{
		assert(false);
	}

	void EditorManager::Init(const EditorParameter& param)
	{
		m_pTabContainer = static_cast<Widgets::TabContainer*>(param.m_pParent);

		m_editors.PushBack(new GamePlayer());
		m_editors.PushBack(new LevelEditor());
		m_editors.PushBack(new MeshEditor());
		m_editors.PushBack(new MaterialEditor());
		m_editors.PushBack(new TextureEditor());

		for (BaseEditor* pEditor : m_editors)
			pEditor->CreateEditor(param);
	}

	void EditorManager::Shutdown()
	{
		for (BaseEditor* pEditor : m_editors)
			delete pEditor;

		m_editors.Clear();
	}

	void EditorManager::SwitchTab(TabId id)
	{
		m_pTabContainer->SetSelectedTab(static_cast<int>(id));
	}
}
