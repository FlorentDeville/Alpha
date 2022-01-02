/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <d3d12.h>

#include "Resource/Resource.h"
#include "Resource/ResourceMgr.h"

struct ID3D12Resource;

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

DECLARE_RESOURCE_ID(Texture);
extern RESOURCE_MGR_PTR(Texture) g_pTextureMgr;
