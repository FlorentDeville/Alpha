/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <d3d12.h>

struct ID3D12Resource;

class Texture
{
public:
	Texture();
	~Texture();

	void Init(const std::string& path);

	const ID3D12Resource* GetResource() const;
	ID3D12Resource* GetResource();
	const D3D12_RESOURCE_DESC& GetResourceDesc() const;

private:
	std::string m_path;
	ID3D12Resource* m_pResource;
	D3D12_RESOURCE_DESC m_resourceDesc;
};
