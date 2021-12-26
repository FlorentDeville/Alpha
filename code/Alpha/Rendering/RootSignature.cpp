/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "RootSignature.h"

#include <d3dcompiler.h>

#include "Helper.h"
#include "RenderModule.h"

extern RenderModule* g_pRenderModule;

RootSignature::RootSignature(const std::string& path)
	: m_path(path)
	, m_pBlob(nullptr)
	, m_pRootSignature(nullptr)
{
	//Load the blob containing the root signature
	std::wstring widePath = std::wstring(path.cbegin(), path.cend());
	ThrowIfFailed(D3DReadFileToBlob(widePath.c_str(), &m_pBlob));

	// Create a root signature.
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(g_pRenderModule->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	// Create the root signature.
	ThrowIfFailed(g_pRenderModule->GetDevice()->CreateRootSignature(0, m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)));
}

RootSignature::~RootSignature()
{
	if (m_pBlob)
		m_pBlob->Release();

	if (m_pRootSignature)
		m_pRootSignature->Release();
}

ID3D12RootSignature* RootSignature::GetRootSignature() const
{
	return m_pRootSignature;
}
