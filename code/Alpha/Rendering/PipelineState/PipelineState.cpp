/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "PipelineState.h"

#include <d3d12.h>
#include <d3dx12.h>

#include "Helper.h"
#include "Rendering/InputLayout/InputLayout.h"
#include "Rendering/RenderModule.h"
#include "Rendering/RootSignature/RootSignature.h"
#include "Rendering/RootSignature/RootSignatureMgr.h"
#include "Rendering/Shader.h"
#include "Rendering/ShaderMgr.h"

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

PipelineState::PipelineState(const std::string& name)
	: Resource(name)
	, m_pPipelineState(nullptr)
	, m_psId()
	, m_rsId()
	, m_vsId()
{}

PipelineState::~PipelineState()
{
	if (m_pPipelineState)
		m_pPipelineState->Release();
}

void PipelineState::Init_Pos(RootSignatureId rsId, ShaderId vsId, ShaderId psId)
{
	m_rsId = rsId;
	m_vsId = vsId;
	m_psId = psId;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	RootSignature* pRootSignature = g_pRootSignatureMgr->GetRootSignature(rsId);

	PipelineStateStream pipelineStateStream;
	pipelineStateStream.pRootSignature = pRootSignature->GetRootSignature();
	pipelineStateStream.InputLayout = { g_inputLayout_pos, _countof(g_inputLayout_pos) };
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(g_pShaderMgr->GetShader(vsId)->GetBlob());
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(g_pShaderMgr->GetShader(psId)->GetBlob());
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
	};
	ThrowIfFailed(g_pRenderModule->GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState)));
}

void PipelineState::Init_PosUv(RootSignatureId rsId, ShaderId vsId, ShaderId psId)
{
	m_rsId = rsId;
	m_vsId = vsId;
	m_psId = psId;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	RootSignature* pRootSignature = g_pRootSignatureMgr->GetRootSignature(rsId);

	PipelineStateStream pipelineStateStream;
	pipelineStateStream.pRootSignature = pRootSignature->GetRootSignature();
	pipelineStateStream.InputLayout = { g_inputLayout_pos_uv, _countof(g_inputLayout_pos_uv) };
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(g_pShaderMgr->GetShader(vsId)->GetBlob());
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(g_pShaderMgr->GetShader(psId)->GetBlob());
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
	};
	ThrowIfFailed(g_pRenderModule->GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState)));
}

void PipelineState::Init_PosColor(RootSignatureId rsId, ShaderId vsId, ShaderId psId)
{
	m_rsId = rsId;
	m_vsId = vsId;
	m_psId = psId;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	RootSignature* pRootSignature = g_pRootSignatureMgr->GetRootSignature(rsId);

	PipelineStateStream pipelineStateStream;
	pipelineStateStream.pRootSignature = pRootSignature->GetRootSignature();
	pipelineStateStream.InputLayout = { g_inputLayout_pos_color, _countof(g_inputLayout_pos_color) };
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(g_pShaderMgr->GetShader(vsId)->GetBlob());
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(g_pShaderMgr->GetShader(psId)->GetBlob());
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
	};
	ThrowIfFailed(g_pRenderModule->GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState)));
}

void PipelineState::Init_Text(RootSignatureId rsId, ShaderId vsId, ShaderId psId)
{
	m_rsId = rsId;
	m_vsId = vsId;
	m_psId = psId;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	RootSignature* pRootSignature = g_pRootSignatureMgr->GetRootSignature(rsId);

	TextPipelineStateStream pipelineStateStream;
	pipelineStateStream.pRootSignature = pRootSignature->GetRootSignature();
	pipelineStateStream.InputLayout = { g_inputLayout_text, _countof(g_inputLayout_text) };
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(g_pShaderMgr->GetShader(vsId)->GetBlob());
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(g_pShaderMgr->GetShader(psId)->GetBlob());
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
	ThrowIfFailed(g_pRenderModule->GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pPipelineState)));
}

ID3D12PipelineState* PipelineState::GetPipelineState() const
{
	return m_pPipelineState;
}

RootSignatureId PipelineState::GetRootSignatureId() const
{
	return m_rsId;
}

RESOURCE_MGR_PTR(PipelineState) g_pPipelineStateMgr = nullptr;
