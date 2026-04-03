/********************************************************************************/
/* Copyright (C) 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "ShaderId.h"

#include <string>
#include <vector>

namespace Rendering
{
	class Shader;

	enum class EngineShaders
	{
		SHADOWMAP_SPOTLIGHT_VS,
		SHADOWMAP_SPOTLIGHT_PS,

		SHADOWMAP_DIRLIGHT_VS,
		SHADOWMAP_DIRLIGHT_PS,

		OBJECTID_VS,
		OBJECTID_PS,

		COPY_RENDER_TARGET_VS,
		COPY_RENDER_TARGET_PS,

		BASE_SHAPE_VS,
		BASE_SHAPE_PS,

		TEXTURE_EDITOR_VIEWPORT_VS,
		TEXTURE_EDITOR_VIEWPORT_PS,

		BLOOM_EMISSIVE_FILTER_VS,
		BLOOM_EMISSIVE_FILTER_PS,
		BLOOM_DOWNSAMPLE_VS,
		BLOOM_DOWNSAMPLE_PS,
		BLOOM_UPSAMPLE_VS,
		BLOOM_UPSAMPLE_PS,

		COUNT
	};

	class ShaderMgr : public Core::Singleton<ShaderMgr>
	{
	public:
		ShaderMgr();
		~ShaderMgr();

		void Init(const std::string& shaderPath);
		void Shutdown();

		ShaderId CreateShader(const std::string& path);

		Shader* GetShader(ShaderId id) const;
		Shader* GetShader(EngineShaders shader) const;

	private:
		std::vector<Shader*> m_shaders;
	};
}