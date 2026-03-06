/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>

namespace Core
{
	class Blob;
}

namespace Editors
{
	class MaterialParameters;
	class RootSignatureDescription;

	class ShaderCompiler
	{
	public:
		ShaderCompiler();
		~ShaderCompiler();

		bool CompileShader(const std::string& shaderSourceFilePath, const std::string& includePath, const std::string& outputFile, bool debug = false);

		bool GenerateRootSignature(const Core::Blob& ps, const Core::Blob& vs, RootSignatureDescription& rs);

		bool GenerateMaterialParameters(const Core::Blob& ps, const Core::Blob& vs, MaterialParameters& parameters);

	private:
		std::string m_shaderCompilerPath;
	};
}
