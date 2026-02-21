/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/TextureEditor/TextureEditor.h"

#include "Core/Log/LogModule.h"

#include "Editors/TextureEditor/TextureEditorModule.h"
#include "Editors/TextureEditor/TextureListModel.h"
#include "Editors/Widgets/Dialog/OkCancelDialog.h"

#include "OsWin/FileDialog.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Assets/Metadata/AssetMetadata.h"

#include "Widgets/Layout.h"
#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Widgets/TableView.h"

//#pragma optimize("", off)

namespace Editors
{
	TextureEditor::TextureEditor()
		: BaseEditor()
		, m_pListModel(nullptr)
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

			Widgets::MenuItem* pDeleteItem = pFileMenu->AddMenuItem("Delete");
			pDeleteItem->SetShortcut("Del");
			pDeleteItem->OnClick([this]() { OnClick_File_Delete(); });
		}

		Widgets::SplitVertical* pVerticalSplit = new Widgets::SplitVertical();
		pVerticalSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pVerticalSplit->SetLeftPanelWidth(500);

		m_pInternalLayout->AddWidget(pVerticalSplit);

		Widgets::TableView* pTable = new Widgets::TableView();
		pVerticalSplit->AddLeftPanel(pTable);

		m_pListModel = new TextureListModel();
		pTable->SetModel(m_pListModel);
		pTable->SetColumnWidth(TextureListModel::Columns::Name, 200);

		TextureEditorModule& textureModule = TextureEditorModule::Get();
		textureModule.OnTextureCreated([this](const Systems::AssetMetadata& metadata) { m_pListModel->AddRow(metadata); });
		textureModule.OnBeforeTextureDeleted([this](const Systems::AssetMetadata& metadata) { m_pListModel->RemoveRow(metadata.GetAssetId()); });
	}

	void TextureEditor::OnClick_File_Import()
	{
		std::string filename;
		bool res = Os::OpenFileDialog(filename);
		if (!res)
			return;

		res = TextureEditorModule::Get().ImportTexture(filename);

		if (!res)
			Core::LogModule::Get().LogError("Failed to import texture %s", filename.c_str());
		else
			Core::LogModule::Get().LogInfo("Texture %s imported.", filename.c_str());
	}

	void TextureEditor::OnClick_File_Delete()
	{
		Systems::NewAssetId selectedTextureId = Systems::NewAssetId::INVALID;

		const std::list<Widgets::SelectionRow>& selectedRows = m_pListModel->GetSelectionModel()->GetSelectedRows();
		for (const Widgets::SelectionRow& row : selectedRows)
		{
			Widgets::ModelIndex index = row.GetStartIndex();
			selectedTextureId = m_pListModel->GetAssetId(index);
			break;
		}

		if (!selectedTextureId.IsValid())
			return;

		Systems::AssetMetadata* pMetadata = Systems::AssetMgr::Get().GetMetadata(selectedTextureId);
		if (!pMetadata)
			return;

		const int BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE] = { '\0' };
		snprintf(buffer, BUFFER_SIZE, "Are you sure you want to delete the texture %s?", pMetadata->GetVirtualName().c_str());

		OkCancelDialog* pDialog = new OkCancelDialog("Delete", buffer);
		pDialog->OnOk([selectedTextureId]() { TextureEditorModule::Get().DeleteTexture(selectedTextureId); });
		pDialog->Open();
	}
}
