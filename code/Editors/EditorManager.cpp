/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/EditorManager.h"

#include "Editors/MaterialEditor/MaterialEditor.h"

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

	void EditorManager::Init(Widgets::Widget* pParent)
	{
		MaterialEditor* pMaterialEditor = new MaterialEditor();
		pMaterialEditor->CreateEditor(pParent);
		m_editors.PushBack(pMaterialEditor);
	}

	void EditorManager::Shutdown()
	{
		for (BaseEditor* pEditor : m_editors)
			delete pEditor;

		m_editors.Clear();
	}
}
