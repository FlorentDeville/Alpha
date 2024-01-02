/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <d3d12.h>

struct ID3D12Resource;

namespace Rendering
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		void Init(const char* ptr, uint32_t size);
		void Init(const std::string& path);
		void Init_RenderTarget(int width, int height, float* clearColor);

		const ID3D12DescriptorHeap* GetSRV() const;
		ID3D12DescriptorHeap* GetSRV();

		ID3D12Resource* GetResource();

		uint64_t GetWidth() const;
		uint64_t GetHeight() const;

	private:
		ID3D12Resource* m_pResource;
		D3D12_RESOURCE_DESC m_resourceDesc;
		ID3D12DescriptorHeap* m_pSrvDescriptorHeap;

		void Internal_Init(const unsigned char* pData, int width, int height, int channel);
	};
}
