/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include <d3d12.h>

class RootSignature
{
public:
	RootSignature(const std::string& path);
	~RootSignature();

	ID3D12RootSignature* GetRootSignature() const;

private:
	std::string m_path;
	ID3D12RootSignature* m_pRootSignature;
};
