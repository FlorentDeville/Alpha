/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/MaterialEditor/Compiler/ShaderCompiler.h"

#include "Core/Collections/Array.h"
#include "Core/Log/LogModule.h"

#include "Editors/MaterialEditor/Compiler/MaterialParameters.h"
#include "Editors/MaterialEditor/Compiler/RootSignatureDescription.h"
#include "OsWin/Process.h"

#include <assert.h>
#include <d3d12.h>
#include <d3dcompiler.h>

namespace Editors
{
	struct BoundResourceConstBuffer
	{
		D3D12_ROOT_PARAMETER m_dx12Parameter; //parameter used to create the root signature
		std::string m_name;
	};

	struct BoundResourceTexture
	{
		D3D12_ROOT_PARAMETER m_dx12Parameter; //parameter used to create the root signature
		std::string m_name;
	};

	struct BoundResourceSample
	{
		D3D12_STATIC_SAMPLER_DESC m_dx12Parameter; //used to create the sample in the root signature
		std::string m_name;
	};

	struct RootSigBoundResources
	{
		Core::Array<BoundResourceConstBuffer> m_constBuffer;
		Core::Array<BoundResourceTexture> m_texture;
		Core::Array<BoundResourceSample> m_sampleParameters;
	};

	enum SHADER_TYPE
	{
		Pixel,
		Vertex,

		Count
	};

	bool GetRootSignatureParameters(ID3D12ShaderReflection* pReflector, RootSigBoundResources& boundResources, SHADER_TYPE shaderType)
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
				BoundResourceConstBuffer constBuffer;
				constBuffer.m_name = bind.Name;

				D3D12_ROOT_PARAMETER& rootParameter = constBuffer.m_dx12Parameter;
				rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				rootParameter.Descriptor.ShaderRegister = bind.BindPoint;
				rootParameter.Descriptor.RegisterSpace = bind.Space;

				if (shaderType == SHADER_TYPE::Pixel)
					rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
				else if (shaderType == SHADER_TYPE::Vertex)
					rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

				boundResources.m_constBuffer.PushBack(constBuffer);
			}
			break;

			case D3D_SIT_TEXTURE:
			{
				BoundResourceTexture boundResource;
				boundResource.m_name = bind.Name;

				D3D12_DESCRIPTOR_RANGE* pDx12Range = new D3D12_DESCRIPTOR_RANGE();
				pDx12Range->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				pDx12Range->NumDescriptors = bind.BindCount; // Only one texture
				pDx12Range->BaseShaderRegister = bind.BindPoint; // The register in the shader (e.g., t0)
				pDx12Range->RegisterSpace = 0;
				pDx12Range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				boundResource.m_dx12Parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				boundResource.m_dx12Parameter.DescriptorTable.NumDescriptorRanges = 1;
				boundResource.m_dx12Parameter.DescriptorTable.pDescriptorRanges = pDx12Range;

				if (shaderType == SHADER_TYPE::Pixel)
					boundResource.m_dx12Parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
				else if (shaderType == SHADER_TYPE::Vertex)
					boundResource.m_dx12Parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

				boundResources.m_texture.PushBack(boundResource);
			}
				break;

			case D3D_SIT_SAMPLER:
			{
				BoundResourceSample boundResourceSample;
	
				memset(&boundResourceSample.m_dx12Parameter, 0, sizeof(D3D12_STATIC_SAMPLER_DESC));

				boundResourceSample.m_dx12Parameter.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				boundResourceSample.m_dx12Parameter.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				boundResourceSample.m_dx12Parameter.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				boundResourceSample.m_dx12Parameter.RegisterSpace = bind.Space;
				boundResourceSample.m_dx12Parameter.ShaderRegister = bind.BindPoint;

				if (shaderType == SHADER_TYPE::Pixel)
					boundResourceSample.m_dx12Parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
				else if(shaderType == SHADER_TYPE::Vertex)
					boundResourceSample.m_dx12Parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

				boundResources.m_sampleParameters.PushBack(boundResourceSample);
			}
			break;

			default:
				return false;
				break;
			}
		}

		return true;
	}

	bool GeneratePerMaterialParameters(ID3D12ShaderReflectionConstantBuffer* pConstantBuffer, const D3D12_SHADER_BUFFER_DESC& bufferDesc, MaterialParameters& parameters)
	{
		parameters.m_perMaterialParameters.reserve(bufferDesc.Variables);
		for (uint32_t ii = 0; ii < bufferDesc.Variables; ++ii)
		{
			ID3D12ShaderReflectionVariable* pVariable = pConstantBuffer->GetVariableByIndex(ii);

			D3D12_SHADER_VARIABLE_DESC variableDesc;
			HRESULT res = pVariable->GetDesc(&variableDesc);
			if (res != S_OK)
				return false;

			ID3D12ShaderReflectionType* pType = pVariable->GetType();
			if (!pType)
				return false;

			D3D12_SHADER_TYPE_DESC typeDesc;
			res = pType->GetDesc(&typeDesc);
			if (res != S_OK)
				return false;

			MaterialParameter parameter;
			parameter.m_name = variableDesc.Name;
			parameter.m_offset = variableDesc.StartOffset;
			parameter.m_size = variableDesc.Size;
			parameter.m_type = SID(typeDesc.Name);
			parameter.m_strType = typeDesc.Name;
			parameters.m_perMaterialParameters.push_back(parameter);
		}
		
		return true;
	}

	bool GenerateMaterialParameters(ID3D12ShaderReflection* pReflector, MaterialParameters& parameters)
	{
		D3D12_SHADER_DESC shaderDesc;
		HRESULT res = pReflector->GetDesc(&shaderDesc);
		if (res != S_OK)
			return false;

		for (uint32_t ii = 0; ii < shaderDesc.ConstantBuffers; ++ii)
		{
			ID3D12ShaderReflectionConstantBuffer* pConstantBuffer = pReflector->GetConstantBufferByIndex(ii);
			if (!pConstantBuffer)
				return false;

			D3D12_SHADER_BUFFER_DESC bufferDesc;
			res = pConstantBuffer->GetDesc(&bufferDesc);
			if (res != S_OK)
				return false;

			if (strcmp(bufferDesc.Name, "PerMaterial") == 0 && parameters.m_perMaterialParameters.empty())
			{
				bool paramRes = GeneratePerMaterialParameters(pConstantBuffer, bufferDesc, parameters);
				if (!paramRes)
					return false;
			}
		}

		return true;
	}

	ShaderCompiler::ShaderCompiler()
		: m_shaderCompilerPath("C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.26100.0\\x64\\fxc.exe")
	{ }

	ShaderCompiler::~ShaderCompiler()
	{ }

	bool ShaderCompiler::CompileShader(const std::string& shaderSourceFilePath, const std::string& includePath, const std::string& outputFile, bool debug)
	{
		const char* PS_EXT = "ps.hlsl";
		const char* VS_EXT = "vs.hlsl";
		const char* RS_EXT = "rs.hlsl";

		size_t extensionSize = strlen(PS_EXT);

		std::string extension = shaderSourceFilePath.substr(shaderSourceFilePath.size() - extensionSize);

		std::string outputName = outputFile;

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

		cmdline += " /I " + includePath;

		if (debug)
		{
			///Od to disable optimizations and /Zi to include debug info
			cmdline += " /Od /Zi";
		}

		Core::LogModule& log = Core::LogModule::Get();
		{
			const int BUFFER_LENGTH = 1024;
			char buffer[BUFFER_LENGTH] = { '\0' };
			snprintf(buffer, BUFFER_LENGTH, "%s\n", cmdline.c_str());
			log.LogInfo(cmdline);
			OutputDebugString(buffer);
		}

		Process shaderCompileProcess(cmdline);

		shaderCompileProcess.OnStdOut([this](const std::string& msg) -> bool 
			{ 
				Core::LogModule::Get().LogInfo(msg);
				const int BUFFER_LENGTH = 1024;
				char buffer[BUFFER_LENGTH] = { '\0' };
				snprintf(buffer, BUFFER_LENGTH, "%s\n", msg.c_str());
				OutputDebugString(buffer);
				return true; 
			});

		bool error = false;
		shaderCompileProcess.OnStdErr([this, &error](const std::string& msg) -> bool 
			{ 
				Core::LogModule::Get().LogError(msg);
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
			log.LogError("Failed to start process");
			OutputDebugString("Failed to start process");
			return false;
		}

		shaderCompileProcess.Wait();

		return !error;
	}

	bool ShaderCompiler::GenerateRootSignature(const Core::Array<char>& ps, const Core::Array<char>& vs, RootSignatureDescription& rs)
	{
		RootSigBoundResources rootSigBoundResources;

		//vertex shader
		{
			ID3D12ShaderReflection* pReflector = nullptr;
			HRESULT res = D3DReflect(vs.GetData(), vs.GetSize(), IID_ID3D12ShaderReflection, (void**)&pReflector);
			if (res != S_OK)
				return false;

			GetRootSignatureParameters(pReflector, rootSigBoundResources, SHADER_TYPE::Vertex);
			pReflector->Release();
		}

		//pixel shader
		{
			ID3D12ShaderReflection* pReflector = nullptr;
			HRESULT res = D3DReflect(ps.GetData(), ps.GetSize(), IID_ID3D12ShaderReflection, (void**)&pReflector);
			if (res != S_OK)
				return false;

			GetRootSignatureParameters(pReflector, rootSigBoundResources, SHADER_TYPE::Pixel);
			pReflector->Release();
		}

		//now create the root signature
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		rootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		rootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		rootSignatureDesc.NumParameters = static_cast<UINT>(rootSigBoundResources.m_constBuffer.GetSize() + rootSigBoundResources.m_texture.GetSize());

		Core::Array<D3D12_ROOT_PARAMETER> rootParameters;
		rootParameters.Reserve(rootSignatureDesc.NumParameters);
		for (const BoundResourceConstBuffer& boundResource : rootSigBoundResources.m_constBuffer)
		{
			RootSigParameterIndex cBuffer;
			cBuffer.m_cbufferName = boundResource.m_name;
			cBuffer.m_rootSigParamIndex = rootParameters.GetSize(); //root param index is different from the bind point (t0, b0). It is the param index in the root signature
			rs.m_parameters.push_back(cBuffer);

			rootParameters.PushBack(boundResource.m_dx12Parameter);
		}
			
		for (const BoundResourceTexture& boundResource : rootSigBoundResources.m_texture)
		{
			rootParameters.PushBack(boundResource.m_dx12Parameter);
		}
		
		rootSignatureDesc.pParameters = rootParameters.GetData();

		rootSignatureDesc.NumStaticSamplers = static_cast<UINT>(rootSigBoundResources.m_sampleParameters.GetSize());

		Core::Array<D3D12_STATIC_SAMPLER_DESC> dx12StaticSamplerArray;
		dx12StaticSamplerArray.Reserve(rootSignatureDesc.NumStaticSamplers);
		for (const BoundResourceSample& boundResource : rootSigBoundResources.m_sampleParameters)
			dx12StaticSamplerArray.PushBack(boundResource.m_dx12Parameter);

		rootSignatureDesc.pStaticSamplers = dx12StaticSamplerArray.GetData();

		ID3DBlob* pBlob = nullptr;
		ID3DBlob* pErrorBlob = nullptr;
		HRESULT res = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pBlob, &pErrorBlob);
		if (res != S_OK)
		{
			char* pError = static_cast<char*>(pErrorBlob->GetBufferPointer());
			OutputDebugString(pError);
			return false;
		}

		rs.m_pRootSignatureBlob->Resize(static_cast<uint32_t>(pBlob->GetBufferSize()));
		memcpy(rs.m_pRootSignatureBlob->GetData(), pBlob->GetBufferPointer(), pBlob->GetBufferSize());

		pBlob->Release();
	
		for (BoundResourceTexture& boundResource : rootSigBoundResources.m_texture)
		{
			delete boundResource.m_dx12Parameter.DescriptorTable.pDescriptorRanges;
		}

		return true;
	}

	bool ShaderCompiler::GenerateMaterialParameters(const Core::Array<char>& ps, const Core::Array<char>& vs, MaterialParameters& parameters)
	{
		//vertex shader
		{
			ID3D12ShaderReflection* pReflector = nullptr;
			HRESULT res = D3DReflect(vs.GetData(), vs.GetSize(), IID_ID3D12ShaderReflection, (void**)&pReflector);
			if (res != S_OK)
				return false;

			bool paramResult = Editors::GenerateMaterialParameters(pReflector, parameters);
			pReflector->Release();

			if (!paramResult)
				return false;
		}

		//pixel shader
		{
			ID3D12ShaderReflection* pReflector = nullptr;
			HRESULT res = D3DReflect(ps.GetData(), ps.GetSize(), IID_ID3D12ShaderReflection, (void**)&pReflector);
			if (res != S_OK)
				return false;

			bool paramResult = Editors::GenerateMaterialParameters(pReflector, parameters);
			pReflector->Release();

			if (!paramResult)
				return false;
		}

		return true;
	}
}
