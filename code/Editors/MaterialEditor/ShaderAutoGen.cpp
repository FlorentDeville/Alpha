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
		std::string cleanFolder = folder;
		if (cleanFolder.back() != '\\' && cleanFolder.back() != '/')
			cleanFolder += '\\';

		std::filesystem::create_directories(cleanFolder);

		WriteCBufferPerFrameHeaderFile(cleanFolder);
		WriteCBufferPerObjectHeaderFile(cleanFolder);
		WriteCBufferLightsHeaderFile(cleanFolder);
		WriteBaseTypesHeaderFile(cleanFolder);
		WriteLightTypesHeaderFile(cleanFolder);
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

		std::string filename = folder + "CBufferPerFrame.hlsl";

		FILE* pFile = nullptr;
		fopen_s(&pFile, filename.c_str(), "w");
		if (!pFile)
			return;

		size_t byteWritten = fwrite(fileContent.c_str(), sizeof(char), fileContent.size(), pFile);
		
		fclose(pFile);
	}

	void ShaderAutoGen::WriteCBufferPerObjectHeaderFile(const std::string& folder)
	{
		std::string fileContent;

		WriteCopyright(fileContent);

		//Write pragma once
		fileContent += "#ifndef CBUFFER_PER_OBJECT_HLSL" + NEW_LINE;
		fileContent += "#define CBUFFER_PER_OBJECT_HLSL" + NEW_LINE + NEW_LINE;

		//set the matrix as row major
		fileContent += "#pragma pack_matrix(row_major)" + NEW_LINE + NEW_LINE;

		fileContent += "#include \"LightTypes.hlsl\"" + NEW_LINE + NEW_LINE;

		//now write the definition of the cbuffer PerFrame
		fileContent += "cbuffer PerObject" + NEW_LINE;
		fileContent += "{" + NEW_LINE;
		fileContent += "\tmatrix worldMatrix;" + NEW_LINE;
		fileContent += "\tmatrix lightSpaceMatrix[MAX_LIGHT_COUNT];" + NEW_LINE;
		fileContent += "};" + NEW_LINE + NEW_LINE;

		fileContent += "#endif // ifndef CBUFFER_PER_OBJECT_HLSL " + NEW_LINE;

		
		std::string filename = folder + "CBufferPerObject.hlsl";

		FILE* pFile = nullptr;
		fopen_s(&pFile, filename.c_str(), "w");
		if (!pFile)
			return;

		size_t byteWritten = fwrite(fileContent.c_str(), sizeof(char), fileContent.size(), pFile);

		fclose(pFile);
	}

	void ShaderAutoGen::WriteCBufferLightsHeaderFile(const std::string& folder)
	{
		std::string fileContent;

		WriteCopyright(fileContent);

		//Write pragma once
		fileContent += "#ifndef CBUFFER_LIGHTS_HLSL" + NEW_LINE;
		fileContent += "#define CBUFFER_LIGHTS_HLSL" + NEW_LINE + NEW_LINE;

		fileContent += "#include \"LightTypes.hlsl\"" + NEW_LINE + NEW_LINE;

		//set the matrix as row major
		fileContent += "#pragma pack_matrix(row_major)" + NEW_LINE + NEW_LINE;

		//write the cbuffer
		fileContent += "cbuffer CBufferLights\n"
			"{\n"
			"\tint numLights;\n"
			"\tLight lightArray[MAX_LIGHT_COUNT];\n"
			"};\n" + NEW_LINE;

		fileContent += "Texture2D shadowMap[MAX_LIGHT_COUNT];" + NEW_LINE;
		fileContent += "SamplerState shadowMapSampler;" + NEW_LINE + NEW_LINE;

		fileContent += "#endif // ifndef CBUFFER_LIGHTS_HLSL " + NEW_LINE;

		std::string filename = folder + "CBufferLights.hlsl";

		FILE* pFile = nullptr;
		fopen_s(&pFile, filename.c_str(), "w");
		if (!pFile)
			return;

		size_t byteWritten = fwrite(fileContent.c_str(), sizeof(char), fileContent.size(), pFile);

		fclose(pFile);
	}

	void ShaderAutoGen::WriteBaseTypesHeaderFile(const std::string& folder)
	{
		std::string fileContent;

		WriteCopyright(fileContent);

		//Write pragma once
		fileContent += "#ifndef BASE_TYPE_HLSL" + NEW_LINE;
		fileContent += "#define BASE_TYPE_HLSL" + NEW_LINE + NEW_LINE;

		//set the matrix as row major
		fileContent += "#pragma pack_matrix(row_major)" + NEW_LINE + NEW_LINE;

		//now write the definition of the cbuffer PerFrame
		fileContent += "typedef float4 color;" + NEW_LINE + NEW_LINE;
		
		fileContent += "#endif // ifndef BASE_TYPE_HLSL" + NEW_LINE;

		std::string filename = folder + "BaseTypes.hlsl";

		FILE* pFile = nullptr;
		fopen_s(&pFile, filename.c_str(), "w");
		if (!pFile)
			return;

		size_t byteWritten = fwrite(fileContent.c_str(), sizeof(char), fileContent.size(), pFile);

		fclose(pFile);
	}

	void ShaderAutoGen::WriteLightTypesHeaderFile(const std::string& folder)
	{
		std::string fileContent;

		WriteCopyright(fileContent);

		//Write pragma once
		fileContent += "#ifndef LIGHT_TYPES_HLSL" + NEW_LINE;
		fileContent += "#define LIGHT_TYPES_HLSL" + NEW_LINE + NEW_LINE;

		
		fileContent += "#define MAX_LIGHT_COUNT 8" + NEW_LINE + NEW_LINE;

		fileContent += "#define DIRECTIONAL_LIGHT 0" + NEW_LINE;
		fileContent += "#define POINT_LIGHT 1" + NEW_LINE;
		fileContent += "#define SPOT_LIGHT 2" + NEW_LINE;

		//write the light structure
		fileContent += "struct Light\n"
			"{\n"
			"\tfloat3 position;\n"
			"\tfloat outerCutOff;\n"
			"\tfloat3 direction;\n\n"
			"\tfloat3 ambient;\n"
			"\tfloat3 diffuse;\n"
			"\tfloat3 specular;\n\n"
			"\tfloat constantAttenuation;\n"
			"\tfloat linearAttenuation;\n"
			"\tfloat quadraticAttenuation;\n\n"
			"\tfloat cutOff;\n\n"
			"\tint type; //0 : directional, 1 : point, 2 : spot\n"
			"};\n" + NEW_LINE;

		fileContent += "#endif // ifndef LIGHT_TYPES_HLSL" + NEW_LINE;

		std::string filename = folder + "LightTypes.hlsl";

		FILE* pFile = nullptr;
		fopen_s(&pFile, filename.c_str(), "w");
		if (!pFile)
			return;

		size_t byteWritten = fwrite(fileContent.c_str(), sizeof(char), fileContent.size(), pFile);

		fclose(pFile);
	}
}
