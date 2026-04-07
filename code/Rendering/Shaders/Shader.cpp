/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Shader.h"

#include "Core/Blob/Blob.h"
#include "Core/Helper.h"

namespace Rendering
{
	Shader::Shader()
		: m_blob()
		, m_path()
	{ }

	Shader::Shader(const std::string& path)
		: m_blob()
		, m_path(path)
	{
		FILE* pFile = nullptr;
		fopen_s(&pFile, path.c_str(), "rb");
		if (!pFile)
			return;

		fseek(pFile, 0, SEEK_END);
		size_t fileSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		//blobs only support 4gb files, which is more than enough for shader.
		assert(fileSize <= UINT32_MAX);

		uint32_t blobSize = static_cast<uint32_t>(fileSize);
		m_blob.Resize(blobSize);
		size_t sizeRead = fread(m_blob.GetData(), sizeof(char), fileSize, pFile);
		fclose(pFile);

		//make sure I read the entire file
		assert(sizeRead == fileSize);
	}

	Shader::~Shader()
	{ }

	bool Shader::LoadFromMemory(const Core::Array<char>& bytecode)
	{
		m_blob.Resize(bytecode.GetSize());
		memcpy(m_blob.GetData(), bytecode.GetData(), bytecode.GetSize());

		return true;
	}

	bool Shader::LoadFromMemory(const Core::Blob& blob)
	{
		m_blob.Resize(blob.GetSize());
		memcpy(m_blob.GetData(), blob.GetData(), blob.GetSize());

		return true;
	}

	const Core::Blob& Shader::GetBlob() const
	{
		return m_blob;
	}
}
