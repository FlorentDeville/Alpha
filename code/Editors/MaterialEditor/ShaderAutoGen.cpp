/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MaterialEditor/ShaderAutoGen.h"

#include <filesystem>

namespace Editors
{
	static const std::string NEW_LINE = "\n";

	static void WriteCopyright(std::string& fileContent)
	{
		fileContent += "/********************************************************************/" + NEW_LINE;
		fileContent += "/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/" + NEW_LINE;
		fileContent += "/********************************************************************/" + NEW_LINE + NEW_LINE;
	}

	void ShaderAutoGen::WriteAllHeaderFiles(const std::string& folder)
	{
		WriteCBufferPerFrameHeaderFile(folder);
	}

	void ShaderAutoGen::WriteCBufferPerFrameHeaderFile(const std::string& folder)
	{
		std::string fileContent;

		WriteCopyright(fileContent);

		//Write pragma once
		fileContent += "#ifndef CBUFFER_PER_FRAME_HLSL" + NEW_LINE;
		fileContent += "#define CBUFFER_PER_FRAME_HLSL" + NEW_LINE + NEW_LINE;

		//set the matrix as row major
		fileContent += "#pragma pack_matrix(row_major)" + NEW_LINE + NEW_LINE;

		//now write the definition of the cbuffer PerFrame
		fileContent += "cbuffer PerFrame" + NEW_LINE;
		fileContent += "{" + NEW_LINE;
		fileContent += "\tmatrix viewMatrix;" + NEW_LINE;
		fileContent += "\tmatrix projMatrix;" + NEW_LINE;
		fileContent += "\tfloat3 cameraPosition;" + NEW_LINE;
		fileContent += "};" + NEW_LINE + NEW_LINE;

		fileContent += "#endif // ifndef CBUFFER_PER_FRAME_HLSL " + NEW_LINE;

		std::filesystem::create_directories(folder);

		std::string filename = folder;
		if (filename.back() != '\\' && filename.back() != '/')
			filename += '\\';

		filename += "CBufferPerFrame.hlsl";

		FILE* pFile = nullptr;
		fopen_s(&pFile, filename.c_str(), "w");
		if (!pFile)
			return;

		size_t byteWritten = fwrite(fileContent.c_str(), sizeof(char), fileContent.size(), pFile);
		
		fclose(pFile);
	}
}
