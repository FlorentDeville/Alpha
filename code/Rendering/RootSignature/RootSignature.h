/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "Core/Collections/Array.h"

struct ID3D12RootSignature;

namespace Rendering
{
	class RootSignature
	{
	public:
		RootSignature();
		RootSignature(const std::string& path);
		~RootSignature();

		ID3D12RootSignature* GetRootSignature() const;

		bool LoadFromMemory(const Core::Array<char>& bytecode);

	private:
		std::string m_path;
		ID3D12RootSignature* m_pRootSignature;
	};
}
