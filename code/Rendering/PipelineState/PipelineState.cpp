/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Rendering/PipelineState/PipelineState.h"

#include <d3d12.h>
#include <d3dx12.h>

#include "Core/Helper.h"

#include "Rendering/InputLayout/InputLayout.h"
#include "Rendering/Internal/BlendFactorToDx12.h"
#include "Rendering/Internal/BlendOperationToDx12.h"
#include "Rendering/Internal/BufferFormatToDx12.h"
#include "Rendering/PipelineState/PipelineStateDesc.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shaders/Shader.h"
#include "Rendering/Shaders/ShaderMgr.h"

namespace Rendering
{
	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	};

	struct TextPipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendState;
	};

	static D3D12_CULL_MODE GetDx12CullMode(CullMode cullMode)
	{
		switch (cullMode)
		{
		case NONE:
			return D3D12_CULL_MODE_NONE;
			break;

		case FRONT:
			return D3D12_CULL_MODE_FRONT;
			break;

		case BACK:
			return D3D12_CULL_MODE_BACK;
			break;
		}

		return D3D12_CULL_MODE_NONE;
	}

	static D3D12_COMPARISON_FUNC GetDx12DepthComparisonMode(DepthComparisonMode mode)
	{
		switch (mode)
		{
		case NEVER:
			return D3D12_COMPARISON_FUNC_NEVER;
			break;

		case LESS:
			return D3D12_COMPARISON_FUNC_LESS;
			break;

		case LESS_OR_EQUAL:
			return D3D12_COMPARISON_FUNC_LESS_EQUAL;
			break;

		case GREATER:
			return D3D12_COMPARISON_FUNC_GREATER;
			break;

		case GREATER_OR_EQUAL:
			return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			break;

		case EQUAL:
			return D3D12_COMPARISON_FUNC_EQUAL;
			break;

		case NOT_EQUAL:
			return D3D12_COMPARISON_FUNC_NOT_EQUAL;
			break;
				
		case ALWAYS:
			return D3D12_COMPARISON_FUNC_ALWAYS;
			break;

		default:
			return D3D12_COMPARISON_FUNC_LESS;
			break;
		}

		return D3D12_COMPARISON_FUNC_LESS;
	}

	PipelineState::PipelineState()
		: m_pPipelineState(nullptr)
		, m_psId()
		, m_rsId()
		, m_vsId()
	{}

	PipelineState::~PipelineState()
	{
		if (m_pPipelineState)
			m_pPipelineState->Release();
	}

	void PipelineState::Init_PosUv(RootSignatureId rsId, ShaderId vsId, ShaderId psId)
	{
		m_rsId = rsId;
		m_vsId = vsId;
		m_psId = psId;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		RootSignature* pRootSignature = Rendering::RootSignatureMgr::Get().GetRootSignature(rsId);
		ShaderMgr& shaderMgr = ShaderMgr::Get();

		const Core::Blob& vsBlob = shaderMgr.GetShader(vsId)->GetBlob();
		const Core::Blob& psBlob = shaderMgr.GetShader(psId)->GetBlob();

		PipelineStateStream pipelineStateStream;
		pipelineStateStream.pRootSignature = pRootSignature->GetRootSignature();
		pipelineStateStream.InputLayout = { g_inputLayout_pos_uv, _countof(g_inputLayout_pos_uv) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vsBlob.GetData(), vsBlob.GetSize());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(psBlob.GetData(), psBlob.GetSize());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		ThrowIfFailed(RenderModule::Get().GetDx12Device()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState)));
	}

	void PipelineState::Init_Text(RootSignatureId rsId, ShaderId vsId, ShaderId psId)
	{
		m_rsId = rsId;
		m_vsId = vsId;
		m_psId = psId;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		RootSignature* pRootSignature = Rendering::RootSignatureMgr::Get().GetRootSignature(rsId);
		ShaderMgr& shaderMgr = ShaderMgr::Get();

		const Core::Blob& vsBlob = shaderMgr.GetShader(vsId)->GetBlob();
		const Core::Blob& psBlob = shaderMgr.GetShader(psId)->GetBlob();

		TextPipelineStateStream pipelineStateStream;
		pipelineStateStream.pRootSignature = pRootSignature->GetRootSignature();
		pipelineStateStream.InputLayout = { g_inputLayout_text, _countof(g_inputLayout_text) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vsBlob.GetData(), vsBlob.GetSize());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(psBlob.GetData(), psBlob.GetSize());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_BLEND_DESC textBlendDesc;
		textBlendDesc.AlphaToCoverageEnable = FALSE;
		textBlendDesc.IndependentBlendEnable = FALSE;
		textBlendDesc.RenderTarget[0].BlendEnable = TRUE;
		textBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
		textBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		textBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA; //D3D12_BLEND_ONE;
		textBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		textBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		textBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		textBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		textBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		CD3DX12_BLEND_DESC blend(textBlendDesc);
		pipelineStateStream.BlendState = blend;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(TextPipelineStateStream), &pipelineStateStream
		};
		ThrowIfFailed(RenderModule::Get().GetDx12Device()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState)));
	}

	void PipelineState::Init_Icon(RootSignatureId rsId, ShaderId vsId, ShaderId psId)
	{
		Init_Icon(rsId, vsId, psId, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	void PipelineState::Init_Icon(RootSignatureId rsId, ShaderId vsId, ShaderId psId, DXGI_FORMAT rtvFormat)
	{
		m_rsId = rsId;
		m_vsId = vsId;
		m_psId = psId;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = rtvFormat;

		RootSignature* pRootSignature = Rendering::RootSignatureMgr::Get().GetRootSignature(rsId);
		ShaderMgr& shaderMgr = ShaderMgr::Get();

		const Core::Blob& vsBlob = shaderMgr.GetShader(vsId)->GetBlob();
		const Core::Blob& psBlob = shaderMgr.GetShader(psId)->GetBlob();

		TextPipelineStateStream pipelineStateStream;
		pipelineStateStream.pRootSignature = pRootSignature->GetRootSignature();
		pipelineStateStream.InputLayout = { g_inputLayout_pos_uv, _countof(g_inputLayout_pos_uv) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vsBlob.GetData(), vsBlob.GetSize());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(psBlob.GetData(), psBlob.GetSize());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_BLEND_DESC textBlendDesc;
		textBlendDesc.AlphaToCoverageEnable = FALSE;
		textBlendDesc.IndependentBlendEnable = FALSE;
		textBlendDesc.RenderTarget[0].BlendEnable = TRUE;
		textBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
		textBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		textBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA; //D3D12_BLEND_ONE;
		textBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		textBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		textBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		textBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		textBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		CD3DX12_BLEND_DESC blend(textBlendDesc);
		pipelineStateStream.BlendState = blend;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(TextPipelineStateStream), &pipelineStateStream
		};
		ThrowIfFailed(RenderModule::Get().GetDx12Device()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState)));
	}

	bool PipelineState::Init_Generic(const PipelineStateDesc& desc)
	{
		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
		};

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = Internal::GetDx12BufferFormat(desc.m_rtvFormat);

		CD3DX12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT());
		rasterizerDesc.CullMode = GetDx12CullMode(desc.m_cullMode);

		CD3DX12_DEPTH_STENCIL_DESC depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());
		depthStencilState.DepthFunc = GetDx12DepthComparisonMode(desc.m_depthFunction);

		CD3DX12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT());
		if (desc.m_blendDesc.m_blendEnabled)
		{
			D3D12_RENDER_TARGET_BLEND_DESC& dx12RenderTargetBlendDesc = blendDesc.RenderTarget[0];
			dx12RenderTargetBlendDesc.BlendEnable = desc.m_blendDesc.m_blendEnabled;
			dx12RenderTargetBlendDesc.SrcBlend = Internal::GetDx12BlendFactor(desc.m_blendDesc.m_srcBlend);
			dx12RenderTargetBlendDesc.DestBlend = Internal::GetDx12BlendFactor(desc.m_blendDesc.m_dstBlend);
			dx12RenderTargetBlendDesc.BlendOp = Internal::GetDx12BlendOperation(desc.m_blendDesc.m_blendOperation);
			dx12RenderTargetBlendDesc.SrcBlendAlpha = Internal::GetDx12BlendFactor(desc.m_blendDesc.m_srcBlendAlpha);
			dx12RenderTargetBlendDesc.DestBlendAlpha = Internal::GetDx12BlendFactor(desc.m_blendDesc.m_dstBlendAlpha);
			dx12RenderTargetBlendDesc.BlendOpAlpha = Internal::GetDx12BlendOperation(desc.m_blendDesc.m_blendOperationAlpha);
		}

		const Core::Blob& vsBlob = desc.m_pVs->GetBlob();
		const Core::Blob& psBlob = desc.m_pPs->GetBlob();

		PipelineStateStream pipelineStateStream;
		pipelineStateStream.Rasterizer = rasterizerDesc;
		pipelineStateStream.pRootSignature = desc.m_pRs->GetRootSignature();
		pipelineStateStream.InputLayout = { g_inputLayout_generic, _countof(g_inputLayout_generic) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vsBlob.GetData(), vsBlob.GetSize());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(psBlob.GetData(), psBlob.GetSize());
		pipelineStateStream.DepthStencilState = depthStencilState;
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;
		pipelineStateStream.BlendDesc = blendDesc;
		
		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		HRESULT res = RenderModule::Get().GetDx12Device()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState));
		return (res == S_OK);
	}

	void PipelineState::Init_Generic_ShadowMap_SpotLight(RootSignature* pRootSig, Shader* pVS, Shader* pPS)
	{
		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		};

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R32_FLOAT;

		CD3DX12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT());
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;

		const Core::Blob& vsBlob = pVS->GetBlob();
		const Core::Blob& psBlob = pPS->GetBlob();

		PipelineStateStream pipelineStateStream;
		pipelineStateStream.Rasterizer = rasterizerDesc;
		pipelineStateStream.pRootSignature = pRootSig->GetRootSignature();
		pipelineStateStream.InputLayout = { g_inputLayout_generic, _countof(g_inputLayout_generic) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vsBlob.GetData(), vsBlob.GetSize());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(psBlob.GetData(), psBlob.GetSize());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		HRESULT res = RenderModule::Get().GetDx12Device()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(res);
	}

	void PipelineState::Init_Generic_ShadowMap_DirLight(RootSignature* pRootSig, Shader* pVS, Shader* pPS)
	{
		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		};

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R32_FLOAT;

		CD3DX12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT());
		rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;

		const Core::Blob& vsBlob = pVS->GetBlob();
		const Core::Blob& psBlob = pPS->GetBlob();

		PipelineStateStream pipelineStateStream;
		pipelineStateStream.Rasterizer = rasterizerDesc;
		pipelineStateStream.pRootSignature = pRootSig->GetRootSignature();
		pipelineStateStream.InputLayout = { g_inputLayout_generic, _countof(g_inputLayout_generic) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vsBlob.GetData(), vsBlob.GetSize());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(psBlob.GetData(), psBlob.GetSize());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		HRESULT res = RenderModule::Get().GetDx12Device()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(res);
	}

	ID3D12PipelineState* PipelineState::GetPipelineState() const
	{
		return m_pPipelineState;
	}

	RootSignatureId PipelineState::GetRootSignatureId() const
	{
		return m_rsId;
	}
}
