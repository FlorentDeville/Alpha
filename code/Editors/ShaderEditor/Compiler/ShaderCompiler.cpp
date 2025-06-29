/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/ShaderEditor/Compiler/ShaderCompiler.h"

#include "OsWin/Process.h"

#include <assert.h>
#include <d3d12.h>
#include <d3dcompiler.h>

namespace Editors
{
	enum SHADER_TYPE
	{
		Pixel,
		Vertex,

		Count
	};

	bool GetRootSignatureParameters(ID3D12ShaderReflection* pReflector, std::vector<D3D12_ROOT_PARAMETER>& rootParameters,
		std::vector<D3D12_STATIC_SAMPLER_DESC>& sampleParameters, SHADER_TYPE shaderType)
	{
		D3D12_SHADER_DESC shaderDesc;
		HRESULT res = pReflector->GetDesc(&shaderDesc);
		if (res != S_OK)
			return false;

		for (uint32_t ii = 0; ii < shaderDesc.BoundResources; ++ii)
		{
			D3D12_SHADER_INPUT_BIND_DESC bind;
			res = pReflector->GetResourceBindingDesc(ii, &bind);
			if (res != S_OK)
				return false;
			
			switch (bind.Type)
			{
			case D3D_SIT_CBUFFER:
			{
				ID3D12ShaderReflectionConstantBuffer* pCBuffer = pReflector->GetConstantBufferByName(bind.Name);
				D3D12_SHADER_BUFFER_DESC bufferDesc;
				pCBuffer->GetDesc(&bufferDesc);

				D3D12_ROOT_PARAMETER rootParameter;
				rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				rootParameter.Descriptor.ShaderRegister = bind.BindPoint;
				rootParameter.Descriptor.RegisterSpace = bind.Space;
				rootParameter.Constants.Num32BitValues = bufferDesc.Size / 4;
				if (shaderType == SHADER_TYPE::Pixel)
					rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
				else if (shaderType == SHADER_TYPE::Vertex)
					rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

				rootParameters.push_back(rootParameter);
			}

				break;

			case D3D_SIT_TEXTURE:
			{
				ID3D12ShaderReflectionConstantBuffer* pCBuffer = pReflector->GetConstantBufferByName(bind.Name);
				D3D12_SHADER_BUFFER_DESC bufferDesc;
				pCBuffer->GetDesc(&bufferDesc);

				D3D12_ROOT_PARAMETER rootParameter;
				rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;

				//this might need to be a descriptor table
				rootParameter.Descriptor.ShaderRegister = bind.BindPoint;
				rootParameter.Descriptor.RegisterSpace = bind.Space;
				if (shaderType == SHADER_TYPE::Pixel)
					rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
				else if (shaderType == SHADER_TYPE::Vertex)
					rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

				rootParameters.push_back(rootParameter);
			}
				break;

			case D3D_SIT_SAMPLER:
			{
				D3D12_STATIC_SAMPLER_DESC sampler;
				memset(&sampler, 0, sizeof(D3D12_STATIC_SAMPLER_DESC));

				sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				sampler.RegisterSpace = bind.Space;
				sampler.ShaderRegister = bind.BindPoint;

				if (shaderType == SHADER_TYPE::Pixel)
					sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
				else if(shaderType == SHADER_TYPE::Vertex)
					sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

				sampleParameters.push_back(sampler);
			}
			break;

			default:
				return false;
				break;
			}
		}

		return true;
	}

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

		bool error = false;
		shaderCompileProcess.OnStdErr([this, &error](const std::string& msg) -> bool 
			{ 
				const int BUFFER_LENGTH = 1024;
				char buffer[BUFFER_LENGTH] = { '\0' };
				snprintf(buffer, BUFFER_LENGTH, "%s\n", msg.c_str());
				OutputDebugString(buffer);
				error = true;
				return true; 
			});

		bool started = shaderCompileProcess.Run();
		if (!started)
		{
			OutputDebugString("Failed to start process");
			return false;
		}

		shaderCompileProcess.Wait();
		return !error;
	}

	bool ShaderCompiler::GenerateRootSignature(const Core::Array<char>& ps, const Core::Array<char>& vs, Core::Array<char>& rs)
	{
		std::vector<D3D12_ROOT_PARAMETER> rootParameters;
		std::vector< D3D12_STATIC_SAMPLER_DESC> staticSamples;

		//vertex shader
		{
			ID3D12ShaderReflection* pReflector = nullptr;
			HRESULT res = D3DReflect(vs.GetData(), vs.GetSize(), IID_ID3D12ShaderReflection, (void**)&pReflector);
			if (res != S_OK)
				return false;

			GetRootSignatureParameters(pReflector, rootParameters, staticSamples, SHADER_TYPE::Vertex);
			pReflector->Release();
		}

		//pixel shader
		{
			ID3D12ShaderReflection* pReflector = nullptr;
			HRESULT res = D3DReflect(ps.GetData(), ps.GetSize(), IID_ID3D12ShaderReflection, (void**)&pReflector);
			if (res != S_OK)
				return false;

			GetRootSignatureParameters(pReflector, rootParameters, staticSamples, SHADER_TYPE::Pixel);
			pReflector->Release();
		}

		//now create the root signature
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		rootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		rootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		rootSignatureDesc.NumParameters = static_cast<UINT>(rootParameters.size());
		rootSignatureDesc.pParameters = rootParameters.data();

		rootSignatureDesc.NumStaticSamplers = static_cast<UINT>(staticSamples.size());
		rootSignatureDesc.pStaticSamplers = staticSamples.data();

		ID3DBlob* pBlob = nullptr;
		ID3DBlob* pErrorBlob = nullptr;
		HRESULT res = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pBlob, &pErrorBlob);
		if (res != S_OK)
		{
			char* pError = static_cast<char*>(pErrorBlob->GetBufferPointer());
			return false;
		}

		rs.Resize(static_cast<uint32_t>(pBlob->GetBufferSize()));
		memcpy(rs.GetData(), pBlob->GetBufferPointer(), pBlob->GetBufferSize());

		pBlob->Release();

		return true;
	}
}
