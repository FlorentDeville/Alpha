/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>

#include "Core/Collections/Array.h"
#include "Core/Blob/Blob.h"

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

		const Core::Blob& GetBlob() const;

	private:
		std::string m_path;
		Core::Blob m_blob;
	};
}
