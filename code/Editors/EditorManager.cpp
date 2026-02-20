/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/EditorManager.h"

#include "Editors/LevelEditor/Widgets/LevelEditor.h"
#include "Editors/MaterialEditor/MaterialEditor.h"
#include "Editors/MeshEditor/MeshEditor.h"
#include "Editors/TextureEditor/TextureEditor.h"

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
		m_editors.PushBack(new LevelEditor());
		m_editors.PushBack(new MeshEditor());
		m_editors.PushBack(new MaterialEditor());
		m_editors.PushBack(new TextureEditor());

		for (BaseEditor* pEditor : m_editors)
			pEditor->CreateEditor(pParent);
	}

	void EditorManager::Shutdown()
	{
		for (BaseEditor* pEditor : m_editors)
			delete pEditor;

		m_editors.Clear();
	}
}
