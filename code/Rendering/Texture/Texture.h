/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <d3d12.h>

#include "Core/Resource/Resource.h"

struct ID3D12Resource;

using TextureId = size_t;

class Texture : public Resource
{
public:
	Texture(const std::string& name);
	~Texture();

	void Init(const std::string& path);
	void Init_RenderTarget(int width, int height);

	const ID3D12DescriptorHeap* GetSRV() const;
	ID3D12DescriptorHeap* GetSRV();

	ID3D12Resource* GetResource();

	uint64_t GetWidth() const;
	uint64_t GetHeight() const;

private:
	ID3D12Resource* m_pResource;
	D3D12_RESOURCE_DESC m_resourceDesc;
	ID3D12DescriptorHeap* m_pSrvDescriptorHeap;
};
