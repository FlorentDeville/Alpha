/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ShaderEditor/ShaderEditor.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Split2Way.h"
#include "Widgets/Tab.h"
#include "Widgets/TabContainer.h"

#include <filesystem>
#include <Windows.h>

namespace Editors
{
	ShaderEntry::ShaderEntry()
		: m_filename()
	{}

	ShaderEditor::ShaderEditor()
		: Core::Singleton<ShaderEditor>()
		, m_allShaders()
		, m_pShaderListLayout(nullptr)
		, m_selectedShader(-1)
	{}

	ShaderEditor::~ShaderEditor()
	{}

	void ShaderEditor::CreateEditor(Widget* pParent)
	{
		//create the widgets
		Widgets::Tab* pViewportTab = new Widgets::Tab();
		Widgets::TabContainer* pTabContainer = dynamic_cast<Widgets::TabContainer*>(pParent);
		if (pTabContainer)
		{
			pTabContainer->AddTab("Shader", pViewportTab);
		}
		else
		{
			pParent->AddWidget(pViewportTab);
		}

		//create the split
		Widgets::Split2Way* pSplit = new Widgets::Split2Way();
		pSplit->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);

		pViewportTab->AddWidget(pSplit);

		//create the list of shaders
		std::string meshRoot = "c:\\workspace\\Alpha\\raw\\shaders";
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(meshRoot))
		{
			m_allShaders.push_back(ShaderEntry());
			m_allShaders.back().m_filename = entry.path().string();
		}

		//create a button and label per shader
		m_pShaderListLayout = new Widgets::Layout(0, 0, 0, 0);
		m_pShaderListLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		m_pShaderListLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pSplit->AddLeftPanel(m_pShaderListLayout);

		for (int ii = 0; ii < m_allShaders.size(); ++ii)
		{
			const ShaderEntry& entry = m_allShaders[ii];

			const std::string& shaderName = entry.m_filename;
			Widgets::Button* pButton = new Widgets::Button(0, 20, 0, 0);
			pButton->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			pButton->OnClick([this, ii](int x, int y) -> bool { return OnShaderEntryClicked(ii); });
			m_pShaderListLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, shaderName);
			pButton->AddWidget(pLabel);
		}

		//right panel : add option layout
		Widgets::Layout* pShaderOptionLayout = new Widgets::Layout(0, 0, 0, 0);
		pShaderOptionLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		pShaderOptionLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pSplit->AddRightPanel(pShaderOptionLayout);

		//add compile button
		{
			Widgets::Button* pButton = new Widgets::Button(0, 20, 0, 0);
			pButton->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			pButton->OnClick([this](int x, int y) -> bool { return OnCompileClicked(); });
			pShaderOptionLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, "Compile");
			pButton->AddWidget(pLabel);
		}

		//add open button
		{
			Widgets::Button* pButton = new Widgets::Button(0, 20, 0, 0);
			pButton->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_DEFAULT);
			//pButton->OnClick([this, ii](int x, int y) -> bool { OnMeshEntryClicked(ii); return true; });
			pShaderOptionLayout->AddWidget(pButton);

			Widgets::Label* pLabel = new Widgets::Label(0, 0, 1, "Open...");
			pButton->AddWidget(pLabel);
		}
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

		std::string extension = shader.m_filename.substr(shader.m_filename.size() - extensionSize);
		size_t nameStartPos = shader.m_filename.find_last_of('\\');
		std::string shaderName = shader.m_filename.substr(nameStartPos + 1, shader.m_filename.size() - extensionSize - nameStartPos - 1);
		std::string shaderTypeExtension = extension.substr(1, 2);
		std::string outputName = "C:\\workspace\\Alpha\\data\\shaders\\" + shaderName + "." + shaderTypeExtension + ".cso";

		//create the command line
		std::string cmdline = "C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.19041.0\\x86\\fxc.exe";

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
		cmdline += " \"" + shader.m_filename + "\"";

		{
			const int BUFFER_LENGTH = 1024;
			char buffer[BUFFER_LENGTH] = { '\0' };
			snprintf(buffer, BUFFER_LENGTH, "%s\n", cmdline.c_str());
			OutputDebugString(buffer);
		}

		//start process
		STARTUPINFO info = { sizeof(info) };
		PROCESS_INFORMATION processInfo;
		bool processCreated = CreateProcess(NULL, cmdline.data(), NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo);
		if (!processCreated)
		{
			OutputDebugString("Failed to start process\n");
			return true;
		}

		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		OutputDebugString("Process over\n");
		//HANDLE WaitHandles[] = { processInfo.hProcess, info.hStdOutput, info.hStdError };

		//const DWORD BUFSIZE = 4096;
		//BYTE buff[BUFSIZE];

		//while (1)
		//{
		//	DWORD dwBytesRead, dwBytesAvailable;

		//	//DWORD dwWaitResult = WaitForMultipleObjects(3, WaitHandles, FALSE, INFINITE);

		//	// Read from the pipes...
		//	while (PeekNamedPipe(info.hStdOutput, NULL, 0, NULL, &dwBytesAvailable, NULL) && dwBytesAvailable)
		//	{
		//		ReadFile(info.hStdOutput, buff, BUFSIZE - 1, &dwBytesRead, 0);
		//		std::string output((char*)buff, (size_t)dwBytesRead);
		//		OutputDebugString(output.c_str());
		//	}

		//	while (PeekNamedPipe(info.hStdError, NULL, 0, NULL, &dwBytesAvailable, NULL) && dwBytesAvailable)
		//	{
		//		ReadFile(info.hStdError, buff, BUFSIZE - 1, &dwBytesRead, 0);
		//		std::string outputErr((char*)buff, (size_t)dwBytesRead);
		//		OutputDebugString(outputErr.c_str());
		//	}

		//	// Process is done, or we timed out:
		//	if (dwWaitResult == WAIT_OBJECT_0 || dwWaitResult == WAIT_TIMEOUT)
		//		break;
		//}

		//CloseHandle(processInfo.hProcess);
		//CloseHandle(processInfo.hThread);

		return true;

	}
}
