/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/TextureEditor/TextureEditor.h"

#include "Core/Log/LogModule.h"

#include "Editors/TextureEditor/TextureEditorModule.h"
#include "Editors/TextureEditor/TextureListModel.h"

#include "OsWin/FileDialog.h"

#include "Widgets/Layout.h"
#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Widgets/TableView.h"

//#pragma optimize("", off)

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

		Widgets::SplitVertical* pVerticalSplit = new Widgets::SplitVertical();
		pVerticalSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pVerticalSplit->SetLeftPanelWidth(500);

		m_pInternalLayout->AddWidget(pVerticalSplit);

		Widgets::TableView* pTable = new Widgets::TableView();
		pVerticalSplit->AddLeftPanel(pTable);

		TextureListModel* pListModel = new TextureListModel();
		pTable->SetModel(pListModel);
		pTable->SetColumnWidth(TextureListModel::Columns::Name, 200);

		TextureEditorModule::Get().OnTextureCreated([pListModel](const Systems::AssetMetadata& metadata) { pListModel->AddRow(metadata); });
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
