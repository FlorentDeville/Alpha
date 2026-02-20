/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Collections/Array.h"
#include "Core/Singleton.h"

namespace Widgets
{
	class Widget;
}

namespace Editors
{
	class BaseEditor;

	class EditorManager : public Core::Singleton<EditorManager>
	{
	public:
		EditorManager();
		~EditorManager();

		void Init() override;
		void Init(Widgets::Widget* pParent);
		void Shutdown() override;

	private:
		Core::Array<BaseEditor*> m_editors;
	};
}

