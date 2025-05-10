/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ShaderEditor/ShaderEditor.h"

#include "Editors/ShaderEditor/ShaderEditorModule.h"
#include "Editors/Widgets/Dialog/UserInputDialog.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridPopulator.h"
#include "Editors/Widgets/PropertyGrid/PropertyGridWidget.h"

#include "OsWin/Process.h"

#include "Systems/Assets/AssetMgr.h"
#include "Systems/Container/Container.h"
#include "Systems/Container/ContainerMgr.h"
#include "Systems/Objects/AssetObject.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Menu.h"
#include "Widgets/MenuBar.h"
#include "Widgets/MenuItem.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Text.h"

#include <filesystem>

namespace Editors
{
	ShaderEditor::ShaderEditor()
		: Core::Singleton<ShaderEditor>()
		, m_allShaders()
		, m_pShaderListLayout(nullptr)
		, m_selectedShader(-1)
		, m_pLogText(nullptr)
	{}

	ShaderEditor::~ShaderEditor()
	{}

	void ShaderEditor::CreateEditor(const ShaderEditorParameter& parameter)
	{
		//create the widgets
		Widgets::Tab* pViewportTab = new Widgets::Tab();
		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(parameter.m_pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab("Shader", pViewportTab);
		}
		else
		{
			parameter.m_pParent->AddWidget(pViewportTab);
		}

		Widgets::Layout* pInternalLayout = new Widgets::Layout();
		pInternalLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pInternalLayout->SetSizeStyle(Widgets::Widget::SIZE_STYLE::STRETCH);
		pViewportTab->AddWidget(pInternalLayout);

		CreateMenu(pInternalLayout);

		//create the split
		Widgets::SplitVertical* pSplit = new Widgets::SplitVertical();
		pSplit->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pSplit->SetLeftPanelWidth(500);

		pInternalLayout->AddWidget(pSplit);

		//create a button and label per shader
		m_pShaderListLayout = new Widgets::Layout(0, 0, 0, 0);
		m_pShaderListLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		m_pShaderListLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pSplit->AddLeftPanel(m_pShaderListLayout);

		CreateShadersList();

		//right panel : add option layout
		Widgets::Layout* pRightPanelLayout = new Widgets::Layout(0, 0, 0, 0);
		pRightPanelLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		pRightPanelLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pSplit->AddRightPanel(pRightPanelLayout);

		//horizontal layout for buttons
		Widgets::Layout* pButtonLayout = new Widgets::Layout(0, 20, 0, 0);
		pButtonLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
		pButtonLayout->SetDirection(Widgets::Layout::Direction::Horizontal);
		pRightPanelLayout->AddWidget(pButtonLayout);

		//add compile button
		{
			Widgets::Button* pButton = new Widgets::Button(60, 0, 0, 0);
			pButton->SetSizeStyle(Widgets::Widget::VSIZE_STRETCH);
			pButton->OnClick([this]() -> bool { return OnCompileClicked(); });
			pButtonLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, "Compile");
			pButton->AddWidget(pLabel);
		}

		//add open button
		{
			Widgets::Button* pButton = new Widgets::Button(50, 0, 0, 0);
			pButton->SetSizeStyle(Widgets::Widget::VSIZE_STRETCH);
			//pButton->OnClick([this, ii](int x, int y) -> bool { OnMeshEntryClicked(ii); return true; });
			pButtonLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, "Open...");
			pButton->AddWidget(pLabel);
		}

		{
			Widgets::Button* pButton = new Widgets::Button(50, 0, 0, 0);
			pButton->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH);
			pRightPanelLayout->AddWidget(pButton);
			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, "Button");
			pButton->AddWidget(pLabel);
		}

		//add property grid
		{
			m_pPropertyGrid = new PropertyGridWidget();
			pRightPanelLayout->AddWidget(m_pPropertyGrid);
		}

		//add log label
		m_pLogText = new Widgets::Text(1, "");
		//m_pLogText->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_STRETCH);
		//pRightPanelLayout->AddWidget(m_pLogText);
	}

	void ShaderEditor::Update()
	{}

	void ShaderEditor::Render()
	{}

	void ShaderEditor::CreateMenu(Widgets::Widget* pParent)
	{
		Widgets::MenuBar* pMenuBar = new Widgets::MenuBar();
		pParent->AddWidget(pMenuBar);

		//create the file menu
		{
			Widgets::Menu* pFileMenu = pMenuBar->AddMenu("File");

			Widgets::MenuItem* pNewItem = pFileMenu->AddMenuItem("New Shader...");
			pNewItem->OnClick([this]() { OnMenuFile_NewShader_Clicked(); });

			Widgets::MenuItem* pSaveItem = pFileMenu->AddMenuItem("Save Shader");
			pSaveItem->OnClick([this]() { OnMenuFile_Save_Clicked(); });
		}
	}

	void ShaderEditor::OnMenuFile_NewShader_Clicked()
	{
		//modal windows are automatically deleted when closed,so no need to delete the dialog.
		UserInputDialog* pDialog = new UserInputDialog("New Asset Name");
		pDialog->OnInputValidated([this](const std::string& input) 
			{ 
				ShaderEditorModule::Get().NewShader(input); 
				CreateShadersList();
			});
		pDialog->Open();
	}

	void ShaderEditor::OnMenuFile_Save_Clicked()
	{
		if (m_selectedShader == -1)
			return;

		const ShaderEntry& entry = m_allShaders[m_selectedShader];
		ShaderEditorModule::Get().SaveShader(entry.m_id);
		
	}

	bool ShaderEditor::OnShaderEntryClicked(int index)
	{
		//deselect all buttons
		const std::vector<Widgets::Widget*>& allButtons = m_pShaderListLayout->GetChildren();
		for (Widgets::Widget* pWidget : allButtons)
		{
			Widgets::Button* pButton = static_cast<Widgets::Button*>(pWidget);
			pButton->Unselect();
		}

		//select newly clicked button
		Widgets::Widget* pSelectedButton = allButtons[index];
		Widgets::Button* pButton = static_cast<Widgets::Button*>(pSelectedButton);
		pButton->Select();

		m_selectedShader = index;

		m_pPropertyGrid->ClearAllItems();

		{
			const ShaderEntry& entry = m_allShaders[m_selectedShader];

			Systems::ContainerMgr& containerMgr = Systems::ContainerMgr::Get();
			Systems::Container* pContainer = containerMgr.GetContainer(entry.m_id.GetContainerId());
			if (!pContainer)
			{
				pContainer = containerMgr.LoadContainer(entry.m_id.GetContainerId());
			}

			if (!pContainer)
				return true;

			Systems::AssetObject* pObject = pContainer->GetAsset(entry.m_id.GetObjectId());
			if (!pObject)
				return true;

			PropertyGridPopulator populator;
			populator.Populate(m_pPropertyGrid, pObject);
		}

		return true;
	}

	bool ShaderEditor::OnCompileClicked()
	{
		//if (m_selectedShader == -1)
		//	return true;

		//const ShaderEntry& shader = m_allShaders[m_selectedShader];

		//const char* PS_EXT = ".ps.hlsl";
		//const char* VS_EXT = ".vs.hlsl";
		//const char* RS_EXT = ".rs.hlsl";

		//size_t extensionSize = strlen(PS_EXT);

		//std::string extension = shader.m_rawFilename.substr(shader.m_rawFilename.size() - extensionSize);
		//size_t nameStartPos = shader.m_rawFilename.find_last_of('\\');
		//std::string shaderName = shader.m_rawFilename.substr(nameStartPos + 1, shader.m_rawFilename.size() - extensionSize - nameStartPos - 1);
		//std::string shaderTypeExtension = extension.substr(1, 2);
		//std::string outputName = m_dataShaderPath + "\\" + shader.m_assetId.ToString();


		//std::string input = m_rawShaderPath + "\\" + shader.m_rawFilename;

		////create the command line
		//std::string cmdline = m_shaderCompilerPath;

		//if (strcmp(RS_EXT, extension.c_str()) == 0) //root signature
		//{
		//	cmdline += " /E\"RS\"";
		//	cmdline += " /T\"rootsig_1_1\"";
		//}
		//else if (strcmp(PS_EXT, extension.c_str()) == 0)
		//{
		//	cmdline += " /E\"main\"";
		//	cmdline += " /T\"ps_5_1\"";
		//}
		//else if (strcmp(VS_EXT, extension.c_str()) == 0)
		//{
		//	cmdline += " /E\"main\"";
		//	cmdline += " /T\"vs_5_1\"";
		//}
		//else
		//{
		//	assert(false && " Unknown shader type");
		//}

		//cmdline += " /Fo\"" + outputName + "\"";
		//cmdline += " /nologo";
		//cmdline += " \"" + input + "\"";

		//m_pLogText->AppendText(cmdline + "\n");
		//{
		//	const int BUFFER_LENGTH = 1024;
		//	char buffer[BUFFER_LENGTH] = { '\0' };
		//	snprintf(buffer, BUFFER_LENGTH, "%s\n", cmdline.c_str());
		//	OutputDebugString(buffer);
		//}

		//Process shaderCompileProcess(cmdline);
		//shaderCompileProcess.OnStdOut([this](const std::string& msg) -> bool { m_pLogText->AppendText(msg); return true; });
		//shaderCompileProcess.OnStdErr([this](const std::string& msg) -> bool { m_pLogText->AppendText(msg); return true; });

		//bool started = shaderCompileProcess.Run();
		//if (!started)
		//{
		//	OutputDebugString("Failed to start process");
		//	return true;
		//}

		//shaderCompileProcess.Wait();
		return true;
	}

	void ShaderEditor::CreateShadersList()
	{
		m_allShaders.clear();
		m_pShaderListLayout->DeleteAllChildren();

		const Systems::AssetMgr& assetMgr = Systems::AssetMgr::Get();

		assetMgr.ForEachMetadata([this](const Systems::AssetMetadata& metadata)
			{
				if (metadata.GetAssetType() != MAKESID("Material"))
					return;

				m_allShaders.push_back(ShaderEntry());
				ShaderEntry& entry = m_allShaders.back();
				entry.m_id = metadata.GetAssetId();
				entry.m_name = metadata.GetVirtualName();
			});

		for (int ii = 0; ii < m_allShaders.size(); ++ii)
		{
			const ShaderEntry& entry = m_allShaders[ii];

			Widgets::Button* pButton = new Widgets::Button(0, 20, 0, 0);
			pButton->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
			pButton->OnClick([this, ii]() -> bool { return OnShaderEntryClicked(ii); });
			m_pShaderListLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, entry.m_name);
			pButton->AddWidget(pLabel);
		}
	}
}
