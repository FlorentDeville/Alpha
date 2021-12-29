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

	const ID3D12Resource* GetResource() const;
	ID3D12Resource* GetResource();
	const D3D12_RESOURCE_DESC& GetResourceDesc() const;

private:
	ID3D12Resource* m_pResource;
	D3D12_RESOURCE_DESC m_resourceDesc;
};

DECLARE_RESOURCE_ID(Texture);
extern RESOURCE_MGR_PTR(Texture) g_pTextureMgr;
