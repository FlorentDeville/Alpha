/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ShaderEditor/ShaderEditor.h"

#include "OsWin/Process.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/SplitVertical.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"
#include "Widgets/Text.h"

#include <filesystem>

namespace Editors
{
	ShaderEntry::ShaderEntry()
		: m_rawFilename()
		, m_assetId()
	{}

	ShaderEditor::ShaderEditor()
		: Core::Singleton<ShaderEditor>()
		, m_allShaders()
		, m_pShaderListLayout(nullptr)
		, m_selectedShader(-1)
		, m_pLogText(nullptr)
		, m_shaderRawDb()
	{}

	ShaderEditor::~ShaderEditor()
	{}

	void ShaderEditor::CreateEditor(const ShaderEditorParameter& parameter)
	{
		m_dataShaderPath = parameter.m_dataShaderPath;
		m_shaderCompilerPath = parameter.m_shaderCompilerPath;
		m_rawShaderPath = parameter.m_rawShaderPath;

		LoadRawDb(parameter.m_rawShaderPath + "\\db.txt", m_shaderRawDb);

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

		//create the split
		Widgets::SplitVertical* pSplit = new Widgets::SplitVertical();
		pSplit->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pSplit->SetLeftPanelWidth(500);

		pViewportTab->AddWidget(pSplit);

		//create the list of shaders
		for(const std::pair<Systems::AssetId, std::string>& it : m_shaderRawDb)
		{
			m_allShaders.push_back(ShaderEntry());
			m_allShaders.back().m_rawFilename = it.second;
			m_allShaders.back().m_assetId = Systems::AssetId(it.first);
		}

		//create a button and label per shader
		m_pShaderListLayout = new Widgets::Layout(0, 0, 0, 0);
		m_pShaderListLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		m_pShaderListLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pSplit->AddLeftPanel(m_pShaderListLayout);

		for (int ii = 0; ii < m_allShaders.size(); ++ii)
		{
			const ShaderEntry& entry = m_allShaders[ii];

			const std::string& shaderName = entry.m_rawFilename;
			Widgets::Button* pButton = new Widgets::Button(0, 20, 0, 0);
			pButton->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			pButton->OnClick([this, ii](int x, int y) -> bool { return OnShaderEntryClicked(ii); });
			m_pShaderListLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, shaderName);
			pButton->AddWidget(pLabel);
		}

		//right panel : add option layout
		Widgets::Layout* pRightPanelLayout = new Widgets::Layout(0, 0, 0, 0);
		pRightPanelLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pRightPanelLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pSplit->AddRightPanel(pRightPanelLayout);

		//horizontal layout for buttons
		Widgets::Layout* pButtonLayout = new Widgets::Layout(0, 20, 0, 0);
		pButtonLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
		pButtonLayout->SetDirection(Widgets::Layout::Direction::Horizontal);
		pRightPanelLayout->AddWidget(pButtonLayout);

		//add compile button
		{
			Widgets::Button* pButton = new Widgets::Button(60, 0, 0, 0);
			pButton->SetSizeStyle(Widget::VSIZE_STRETCH);
			pButton->OnClick([this](int x, int y) -> bool { return OnCompileClicked(); });
			pButtonLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, "Compile");
			pButton->AddWidget(pLabel);
		}

		//add open button
		{
			Widgets::Button* pButton = new Widgets::Button(50, 0, 0, 0);
			pButton->SetSizeStyle(Widget::VSIZE_STRETCH);
			//pButton->OnClick([this, ii](int x, int y) -> bool { OnMeshEntryClicked(ii); return true; });
			pButtonLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, "Open...");
			pButton->AddWidget(pLabel);
		}

		//add log label
		m_pLogText = new Widgets::Text(1, "");
		m_pLogText->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pRightPanelLayout->AddWidget(m_pLogText);
	}

	void ShaderEditor::Update()
	{}

	void ShaderEditor::Render()
	{}

	bool ShaderEditor::OnShaderEntryClicked(int index)
	{
		//deselect all buttons
		const std::vector<Widget*>& allButtons = m_pShaderListLayout->GetChildren();
		for (Widget* pWidget : allButtons)
		{
			Widgets::Button* pButton = static_cast<Widgets::Button*>(pWidget);
			pButton->Unselect();
		}

		//select newly clicked button
		Widget* pSelectedButton = allButtons[index];
		Widgets::Button* pButton = static_cast<Widgets::Button*>(pSelectedButton);
		pButton->Select();

		m_selectedShader = index;

		return true;
	}

	bool ShaderEditor::OnCompileClicked()
	{
		if (m_selectedShader == -1)
			return true;

		const ShaderEntry& shader = m_allShaders[m_selectedShader];

		const char* PS_EXT = ".ps.hlsl";
		const char* VS_EXT = ".vs.hlsl";
		const char* RS_EXT = ".rs.hlsl";

		size_t extensionSize = strlen(PS_EXT);

		std::string extension = shader.m_rawFilename.substr(shader.m_rawFilename.size() - extensionSize);
		size_t nameStartPos = shader.m_rawFilename.find_last_of('\\');
		std::string shaderName = shader.m_rawFilename.substr(nameStartPos + 1, shader.m_rawFilename.size() - extensionSize - nameStartPos - 1);
		std::string shaderTypeExtension = extension.substr(1, 2);
		std::string outputName = m_dataShaderPath + "\\" + shader.m_assetId.ToString();


		std::string input = m_rawShaderPath + "\\" + shader.m_rawFilename;

		//create the command line
		std::string cmdline = m_shaderCompilerPath;

		if (strcmp(RS_EXT, extension.c_str()) == 0) //root signature
		{
			cmdline += " /E\"RS\"";
			cmdline += " /T\"rootsig_1_1\"";
		}
		else if (strcmp(PS_EXT, extension.c_str()) == 0)
		{
			cmdline += " /E\"main\"";
			cmdline += " /T\"ps_5_1\"";
		}
		else if (strcmp(VS_EXT, extension.c_str()) == 0)
		{
			cmdline += " /E\"main\"";
			cmdline += " /T\"vs_5_1\"";
		}
		else
		{
			assert(false && " Unknown shader type");
		}

		cmdline += " /Fo\"" + outputName + "\"";
		cmdline += " /nologo";
		cmdline += " \"" + input + "\"";

		m_pLogText->AppendText(cmdline + "\n");
		{
			const int BUFFER_LENGTH = 1024;
			char buffer[BUFFER_LENGTH] = { '\0' };
			snprintf(buffer, BUFFER_LENGTH, "%s\n", cmdline.c_str());
			OutputDebugString(buffer);
		}

		Process shaderCompileProcess(cmdline);
		shaderCompileProcess.OnStdOut([this](const std::string& msg) -> bool { m_pLogText->AppendText(msg); return true; });
		shaderCompileProcess.OnStdErr([this](const std::string& msg) -> bool { m_pLogText->AppendText(msg); return true; });

		bool started = shaderCompileProcess.Run();
		if (!started)
		{
			OutputDebugString("Failed to start process");
			return true;
		}

		shaderCompileProcess.Wait();
		return true;
	}
}
