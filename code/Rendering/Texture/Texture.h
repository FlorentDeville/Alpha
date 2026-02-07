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
		void InitAsRenderTarget(int width, int height, float* clearColor);
		void InitAsReadbackBuffer(int width, int height, int depth);

		void TransitionTo(D3D12_RESOURCE_STATES nextState);

		const ID3D12DescriptorHeap* GetSRV() const;
		ID3D12DescriptorHeap* GetSRV();

		ID3D12Resource* GetResource();
		const D3D12_RESOURCE_DESC& GetResourceDesc() const;

		uint64_t GetWidth() const;
		uint64_t GetHeight() const;

	private:
		ID3D12Resource* m_pResource;
		D3D12_RESOURCE_DESC m_resourceDesc;
		ID3D12DescriptorHeap* m_pSrvDescriptorHeap;

		D3D12_RESOURCE_STATES m_currentState;

		void Internal_Init(const unsigned char* pData, int width, int height, int channel);
	};
}
