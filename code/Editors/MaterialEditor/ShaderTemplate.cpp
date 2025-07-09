/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MaterialEditor/ShaderTemplate.h"

namespace Editors
{
	static const std::string NEW_LINE = "\n";

	static void WriteCopyright(std::string & fileContent)
	{
		fileContent += "/********************************************************************/" + NEW_LINE;
		fileContent += "/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/" + NEW_LINE;
		fileContent += "/********************************************************************/" + NEW_LINE + NEW_LINE;
	}

	static void WriteVertexShaderInput(std::string& content)
	{
		content += "struct VS_Input" + NEW_LINE;
		content += "{" + NEW_LINE;
		content += "\tfloat3 vertex : POSITION;" + NEW_LINE;
		content += "\tfloat3 color : COLOR;" + NEW_LINE;
		content += "\tfloat2 uv : UV;" + NEW_LINE;
		content += "\tfloat3 normal : NORMAL;" + NEW_LINE;
		content += "};" + NEW_LINE + NEW_LINE;
	}

	static void WritePixelShaderInput(std::string& content)
	{
		content += "struct VS_Output" + NEW_LINE;
		content += "{" + NEW_LINE;
		content += "\tfloat4 vertex : SV_Position;" + NEW_LINE;
		content += "\tfloat3 color : COLOR;" + NEW_LINE;
		content += "\tfloat2 uv : UV;" + NEW_LINE;
		content += "\tfloat3 normal : NORMAL;" + NEW_LINE;
		content += "};" + NEW_LINE + NEW_LINE;
	}

	static void WriteFile(const std::string& filename, const std::string& content)
	{
		FILE* pFile = nullptr;
		fopen_s(&pFile, filename.c_str(), "w");
		if (!pFile)
			return;

		size_t byteWritten = fwrite(content.c_str(), sizeof(char), content.size(), pFile);

		fclose(pFile);
	}

	void ShaderTemplate::WriteVertexShaderTemplate(const std::string& filename)
	{
		std::string content;
		WriteCopyright(content);

		//includes
		content += "#include \"CBufferPerObject.hlsl\" \n"
			"#include \"CBufferPerFrame.hlsl\"" + NEW_LINE + NEW_LINE;

		WriteVertexShaderInput(content);
		WritePixelShaderInput(content);

		content += "VS_Output main(VS_Input input) \n"
			"{ \n"
			"\tVS_Output output; \n"
			"\n"
			"\tfloat4 pos = float4(input.vertex, 1.f); \n"
			"\n"
			"\toutput.vertex = mul(pos, worldMatrix); \n"
			"\toutput.vertex = mul(output.vertex, viewMatrix); \n"
			"\toutput.vertex = mul(output.vertex, projMatrix); \n"
			"\n"
			"\toutput.uv = input.uv; \n"
			"\toutput.color = input.color; \n"
			"\n"
			"\tfloat4 normal = float4(input.normal, 0);\n"
			"\toutput.normal = mul(normal, worldMatrix).xyz;\n"
			"\n"
			"\treturn output; \n"
			"} \n";

		WriteFile(filename, content);
	}

	void ShaderTemplate::WritePixelShaderTemplate(const std::string& filename)
	{
		std::string content;
		WriteCopyright(content);

		//includes
		content += "#include \"CBufferLights.hlsl\" \n"
			"#include \"CBufferPerFrame.hlsl\" \n"
			"#include \"BaseTypes.hlsl\"" + NEW_LINE + NEW_LINE;

		//write empty material cbuffer 
		content += "cbuffer PerMaterial \n"
			"{ \n"
			"};" + NEW_LINE + NEW_LINE;

		WritePixelShaderInput(content);

		content += "float4 main(VS_Output input) : SV_TARGET \n"
			"{ \n"
			"	return color(1, 0, 0, 1);\n"
			"}" + NEW_LINE + NEW_LINE;

		WriteFile(filename, content);
	}
}
