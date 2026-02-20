/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/TextureEditor/TextureEditor.h"

namespace Editors
{
	TextureEditor::TextureEditor()
		: BaseEditor()
	{ }

	TextureEditor::~TextureEditor()
	{ }

	void TextureEditor::CreateEditor(Widgets::Widget* pParent)
	{
		CreateDefaultWidgets(pParent, "Texture");
	}
}
