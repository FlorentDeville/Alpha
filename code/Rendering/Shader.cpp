/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Shader.h"

#include <d3dcompiler.h>
#include "Core/Helper.h"

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

ID3DBlob* Shader::GetBlob() const
{
	return m_pBlob;
}
