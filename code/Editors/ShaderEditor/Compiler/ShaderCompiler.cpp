/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ShaderEditor/Compiler/ShaderCompiler.h"

#include "OsWin/Process.h"
#include <assert.h>

namespace Editors
{
	ShaderCompiler::ShaderCompiler()
		: m_shaderCompilerPath("C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.26100.0\\x64\\fxc.exe")
	{ }

	ShaderCompiler::~ShaderCompiler()
	{ }

	bool ShaderCompiler::CompileShader(const std::string& shaderSourceFilePath, const std::string& outputFile)
	{
		const char* PS_EXT = ".ps.hlsl";
		const char* VS_EXT = ".vs.hlsl";
		const char* RS_EXT = ".rs.hlsl";

		size_t extensionSize = strlen(PS_EXT);

		std::string extension = shaderSourceFilePath.substr(shaderSourceFilePath.size() - extensionSize);
		/*size_t nameStartPos = shaderSourceFilePath.find_last_of('\\');
		std::string shaderName = shaderSourceFilePath.substr(nameStartPos + 1, shaderSourceFilePath.size() - extensionSize - nameStartPos - 1);
		std::string shaderTypeExtension = extension.substr(1, 2);

		std::string outputName = outputFolder + "\\" + shader.m_assetId.ToString();*/
		std::string outputName = outputFile;

		//std::string input = m_rawShaderPath + "\\" + shader.m_rawFilename;
		std::string input = shaderSourceFilePath;

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

		//m_pLogText->AppendText(cmdline + "\n");
		{
			const int BUFFER_LENGTH = 1024;
			char buffer[BUFFER_LENGTH] = { '\0' };
			snprintf(buffer, BUFFER_LENGTH, "%s\n", cmdline.c_str());
			OutputDebugString(buffer);
		}

		Process shaderCompileProcess(cmdline);
		//shaderCompileProcess.OnStdOut([this](const std::string& msg) -> bool { m_pLogText->AppendText(msg); return true; });
		//shaderCompileProcess.OnStdErr([this](const std::string& msg) -> bool { m_pLogText->AppendText(msg); return true; });

		shaderCompileProcess.OnStdOut([this](const std::string& msg) -> bool 
			{ 
				const int BUFFER_LENGTH = 1024;
				char buffer[BUFFER_LENGTH] = { '\0' };
				snprintf(buffer, BUFFER_LENGTH, "%s\n", msg.c_str());
				OutputDebugString(buffer);
				return true; 
			});

		shaderCompileProcess.OnStdErr([this](const std::string& msg) -> bool 
			{ 
				const int BUFFER_LENGTH = 1024;
				char buffer[BUFFER_LENGTH] = { '\0' };
				snprintf(buffer, BUFFER_LENGTH, "%s\n", msg.c_str());
				OutputDebugString(buffer);
				return true; 
			});

		bool started = shaderCompileProcess.Run();
		if (!started)
		{
			OutputDebugString("Failed to start process");
			return false;
		}

		shaderCompileProcess.Wait();
		return true;
	}
}
