/********************************************************************/
/* � 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

class Configuration
{
public:
	Configuration();
	~Configuration();

	bool Load(const std::string& binPath, const std::string& filename);
	
	std::string m_binPath;

	std::string m_dataRoot;

	std::string m_rawBlenderPath;
	std::string m_rawShadersPath;

	std::string m_editorsScriptsPath;

	std::string m_shaderCompiler;

	std::string m_blender;

	int m_gameResolutionWidth;
	int m_gameResolutionHeight;
};
