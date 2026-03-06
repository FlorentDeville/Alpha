/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>
#include <d3d12.h>

#include "Core/Collections/Array.h"

namespace Core
{
	class Blob;
}

namespace Rendering
{
	class Shader
	{
	public:
		Shader();
		Shader(const std::string& path);
		~Shader();

		bool LoadFromMemory(const Core::Array<char>& blob);
		bool LoadFromMemory(const Core::Blob& blob);

		ID3DBlob* GetBlob() const;

	private:
		std::string m_path;
		ID3DBlob* m_pBlob;
	};
}
