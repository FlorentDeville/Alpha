/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Shader.h"

#include <d3dcompiler.h>
#include "Core/Helper.h"

namespace Rendering
{
	Shader::Shader()
		: m_pBlob(nullptr)
		, m_path()
	{ }

	Shader::Shader(const std::string& path)
		: m_pBlob(nullptr)
		, m_path(path)
	{
		std::wstring widePath = std::wstring(path.cbegin(), path.cend());
		ThrowIfFailed(D3DReadFileToBlob(widePath.c_str(), &m_pBlob));
	}

	Shader::~Shader()
	{
		if (m_pBlob)
			m_pBlob->Release();
	}

	bool Shader::LoadFromMemory(const Core::Array<char>& bytecode)
	{
		if (m_pBlob)
			m_pBlob->Release();

		HRESULT res = D3DCreateBlob(bytecode.GetSize(), &m_pBlob);
		if (res != S_OK)
			return false;

		memcpy(m_pBlob->GetBufferPointer(), bytecode.GetData(), bytecode.GetSize());

		return true;
	}

	ID3DBlob* Shader::GetBlob() const
	{
		return m_pBlob;
	}
}
