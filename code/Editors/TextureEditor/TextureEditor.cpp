/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/TextureEditor/TextureEditor.h"

#include "Core/Log/LogModule.h"

#include "Editors/TextureEditor/TextureEditorModule.h"

#include "OsWin/FileDialog.h"

#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"

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

		//create the file menu
		{
			Widgets::Menu* pFileMenu = m_pMenuBar->AddMenu("File");

			Widgets::MenuItem* pNewItem = pFileMenu->AddMenuItem("Import...");
			pNewItem->OnClick([this]() { OnClick_File_Import(); });
		}
	}

	void TextureEditor::OnClick_File_Import()
	{
		std::string filename;
		bool res = Os::OpenFileDialog(filename);
		if (!res)
			return;

		res = TextureEditorModule::Get().Import(filename);

		if (!res)
			Core::LogModule::Get().LogError("Failed to import texture %s", filename.c_str());
		else
			Core::LogModule::Get().LogInfo("Texture %s imported.", filename.c_str());
	}
}
