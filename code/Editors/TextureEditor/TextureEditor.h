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
	class TextureListModel;

	class TextureEditor : public BaseEditor
	{
	public:
		TextureEditor();
		~TextureEditor();

		void CreateEditor(Widgets::Widget* pParent) override;

	private:
		TextureListModel* m_pListModel;

		void OnClick_File_Import();
		void OnClick_File_Delete();
	};
}
