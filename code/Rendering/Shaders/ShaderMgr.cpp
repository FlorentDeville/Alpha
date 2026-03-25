/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "ShaderMgr.h"

#include "Shader.h"
#include <assert.h>

namespace Rendering
{
	ShaderMgr::ShaderMgr()
		: m_shaders()
	{}

	ShaderMgr::~ShaderMgr()
	{
		for (Shader* pShader : m_shaders)
			delete pShader;

		m_shaders.clear();
	}

	void ShaderMgr::Init(const std::string& shaderPath)
	{
		m_shaders.resize(static_cast<size_t>(EngineShaders::COUNT));

		m_shaders[static_cast<size_t>(EngineShaders::SHADOWMAP_SPOTLIGHT_VS)] = new Shader(shaderPath + "\\shadowmap.vs.cso");
		m_shaders[static_cast<size_t>(EngineShaders::SHADOWMAP_SPOTLIGHT_PS)] = new Shader(shaderPath + "\\shadowmap.ps.cso");

		m_shaders[static_cast<size_t>(EngineShaders::SHADOWMAP_DIRLIGHT_VS)] = new Shader(shaderPath + "\\shadowmap_dirlight.vs.cso");
		m_shaders[static_cast<size_t>(EngineShaders::SHADOWMAP_DIRLIGHT_PS)] = new Shader(shaderPath + "\\shadowmap_dirlight.ps.cso");
	}

	void ShaderMgr::Shutdown()
	{ }

	ShaderId ShaderMgr::CreateShader(const std::string& path)
	{
		ShaderId id(m_shaders.size());
		m_shaders.push_back(new Shader(path));
		return id;
	}

	Shader* ShaderMgr::GetShader(ShaderId id) const
	{
		assert(id.m_id < m_shaders.size());
		return m_shaders[id.m_id];
	}

	Shader* ShaderMgr::GetShader(EngineShaders shader) const
	{
		return m_shaders[static_cast<size_t>(shader)];
	}
}
