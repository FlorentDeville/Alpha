/********************************************************************/
/* � 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Texture/Texture.h"

#include "Core/Helper.h"
#include "Rendering/CommandQueue.h"
#include "Rendering/RenderModule.h"

#include <d3dx12.h>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image-2.27/stb_image.h"

namespace Rendering
{
	Texture::Texture()
		: m_pResource(nullptr)
		, m_resourceDesc()
		, m_pSrvDescriptorHeap(nullptr)
	{}

	Texture::~Texture()
	{
		if (m_pResource)
			m_pResource->Release();

		if (m_pSrvDescriptorHeap)
			m_pSrvDescriptorHeap->Release();
	}

	void Texture::Init(const char* ptr, uint32_t size)
	{
		int textureWidth = 0;
		int textureHeight = 0;
		int channelCount = 0;
		unsigned char* pData = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(ptr), size, &textureWidth, &textureHeight, &channelCount, 4);

		Internal_Init(pData, textureWidth, textureHeight, channelCount);

		stbi_image_free(pData);
	}

	void Texture::Init(const std::string& path)
	{
		//Load the texture
		int textureWidth = 0;
		int textureHeight = 0;
		int channelCount = 0;
		unsigned char* pData = stbi_load(path.c_str(), &textureWidth, &textureHeight, &channelCount, 4);

		Internal_Init(pData, textureWidth, textureHeight, channelCount);

		stbi_image_free(pData);
	}

	void Texture::Init_RenderTarget(int width, int height, float* clearColor)
	{
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

		//Create a description
		D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		m_resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		//Create the texture
		D3D12_CLEAR_VALUE clear;
		clear.Format = format;
		clear.Color[0] = clearColor[0];
		clear.Color[1] = clearColor[1];
		clear.Color[2] = clearColor[2];
		clear.Color[3] = clearColor[3];

		ID3D12Device* pDevice = RenderModule::Get().GetDevice();
		pDevice->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, &m_resourceDesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET, &clear, IID_PPV_ARGS(&m_pResource));

		//Create the SRV heap
		{
			D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
			srvHeapDesc.NumDescriptors = 1;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			HRESULT res = pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSrvDescriptorHeap));
			ThrowIfFailed(res);
		}

		//Create the srv descriptor
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = m_resourceDesc.Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Texture2D.MipLevels = 1;
			pDevice->CreateShaderResourceView(m_pResource, &srvDesc, m_pSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		}
	}

	const ID3D12DescriptorHeap* Texture::GetSRV() const
	{
		return m_pSrvDescriptorHeap;
	}

	ID3D12DescriptorHeap* Texture::GetSRV()
	{
		return m_pSrvDescriptorHeap;
	}

	ID3D12Resource* Texture::GetResource()
	{
		return m_pResource;
	}

	uint64_t Texture::GetWidth() const
	{
		return m_resourceDesc.Width;
	}

	uint64_t Texture::GetHeight() const
	{
		return m_resourceDesc.Height;
	}

	void Texture::Internal_Init(const unsigned char* pData, int width, int height, int channel)
	{
		ID3D12Device* pDevice = RenderModule::Get().GetDevice();

		//Load the texture
		//int textureWidth = 0;
		//int textureHeight = 0;
		//int channelCount = 0;
		//unsigned char* pData = stbi_load(path.c_str(), &textureWidth, &textureHeight, &channelCount, 4);

		//Create a resource description
		m_resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		m_resourceDesc.Alignment = 0; //let decide dx12 what alignement to use
		m_resourceDesc.Width = width;
		m_resourceDesc.Height = height;
		m_resourceDesc.DepthOrArraySize = 1;
		m_resourceDesc.MipLevels = 1;
		m_resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_resourceDesc.SampleDesc.Count = 1; //number of sample per pixel
		m_resourceDesc.SampleDesc.Quality = 0;
		m_resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		m_resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		{
			//Create the texture resource
			D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			HRESULT res = pDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &m_resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pResource));
			ThrowIfFailed(res);
		}

		//Create the upload resource
		ID3D12Resource* pUploadResource = nullptr;
		{
			UINT64 textureUploadBufferSize = 0;
			pDevice->GetCopyableFootprints(&m_resourceDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);

			D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			D3D12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize);
			HRESULT res = pDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &uploadResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pUploadResource));
			ThrowIfFailed(res);
		}

		//Upload the texture
		{
			CommandQueue* pCopyCommandQueue = RenderModule::Get().GetRenderCommandQueue();
			ID3D12GraphicsCommandList2* pCommandList = pCopyCommandQueue->GetCommandList();

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = pData;
			subresourceData.RowPitch = width * channel * sizeof(char);
			subresourceData.SlicePitch = subresourceData.RowPitch * height;

			UpdateSubresources(pCommandList, m_pResource, pUploadResource, 0, 0, 1, &subresourceData);

			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
			pCommandList->ResourceBarrier(1, &barrier);

			//Execute the commands
			uint64_t fence = pCopyCommandQueue->ExecuteCommandList(pCommandList);
			pCopyCommandQueue->WaitForFenceValue(fence);
		}

		//stbi_image_free(pData);

		//Create the SRV heap
		{
			D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
			srvHeapDesc.NumDescriptors = 1;
			srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			HRESULT res = pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSrvDescriptorHeap));
			ThrowIfFailed(res);
		}

		//Create the srv descriptor
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = m_resourceDesc.Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Texture2D.MipLevels = 1;
			pDevice->CreateShaderResourceView(m_pResource, &srvDesc, m_pSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		}

		pUploadResource->Release();
	}
}
