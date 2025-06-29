/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Editors
{
	class ShaderCompiler
	{
	public:
		ShaderCompiler();
		~ShaderCompiler();

		bool CompileShader(const std::string& shaderSourceFilePath, const std::string& outputFile);

	private:
		std::string m_shaderCompilerPath;
	};
}
