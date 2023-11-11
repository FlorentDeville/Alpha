/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

struct ID3D12RootSignature;

namespace Rendering
{
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
}
