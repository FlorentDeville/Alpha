/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "ShaderId.h"

#include <string>
#include <vector>

class Shader;

class ShaderMgr : public Core::Singleton<ShaderMgr>
{
public:
	ShaderMgr();
	~ShaderMgr();

	ShaderId CreateShader(const std::string& path);

	Shader* GetShader(ShaderId id) const;

private:
	std::vector<Shader*> m_shaders;
};
