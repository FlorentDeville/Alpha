/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/RootSignature/RootSignature.h"

#include <d3dcompiler.h>

#include "Core/Helper.h"
#include "Rendering/RenderModule.h"

namespace Rendering
{
	RootSignature::RootSignature(const std::string& path)
		: m_path(path)
		, m_pRootSignature(nullptr)
	{
		//Load the blob containing the root signature
		ID3DBlob* pBlob = nullptr;
		std::wstring widePath = std::wstring(path.cbegin(), path.cend());
		ThrowIfFailed(D3DReadFileToBlob(widePath.c_str(), &pBlob));

		// Create a root signature.
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(RenderModule::Get().GetDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Create the root signature.
		ThrowIfFailed(RenderModule::Get().GetDevice()->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)));

		pBlob->Release();
	}

	RootSignature::~RootSignature()
	{
		if (m_pRootSignature)
			m_pRootSignature->Release();
	}

	ID3D12RootSignature* RootSignature::GetRootSignature() const
	{
		return m_pRootSignature;
	}
}
