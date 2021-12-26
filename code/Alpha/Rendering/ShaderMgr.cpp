/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "ShaderMgr.h"

#include "Shader.h"
#include <assert.h>

ShaderMgr::ShaderMgr()
	: m_shaders()
{}

ShaderMgr::~ShaderMgr()
{
	for (Shader* pShader : m_shaders)
		delete pShader;
}

ShaderId ShaderMgr::CreateShader(const std::string& path)
{
	ShaderId id;
	id.m_id = m_shaders.size();
	m_shaders.push_back(new Shader(path));
	return id;
}

Shader* ShaderMgr::GetShader(ShaderId id) const
{
	assert(id.m_id < m_shaders.size());
	return m_shaders[id.m_id];
}

ShaderMgr* g_pShaderMgr = nullptr;
