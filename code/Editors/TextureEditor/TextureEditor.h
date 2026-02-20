/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/BaseEditor.h"

namespace Widgets
{
	class Widget;
}

namespace Editors
{
	class TextureEditor : public BaseEditor
	{
	public:
		TextureEditor();
		~TextureEditor();

		void CreateEditor(Widgets::Widget* pParent) override;
	};
}
