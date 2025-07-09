/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "Core/Collections/Array.h"

namespace Editors
{
	class MaterialParameters;
	class RootSignatureDescription;

	class ShaderCompiler
	{
	public:
		ShaderCompiler();
		~ShaderCompiler();

		bool CompileShader(const std::string& shaderSourceFilePath, const std::string& includePath, const std::string& outputFile);

		bool GenerateRootSignature(const Core::Array<char>& ps, const Core::Array<char>& vs, RootSignatureDescription& rs);

		bool GenerateMaterialParameters(const Core::Array<char>& ps, const Core::Array<char>& vs, MaterialParameters& parameters);

	private:
		std::string m_shaderCompilerPath;
	};
}
