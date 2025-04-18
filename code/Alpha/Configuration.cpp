/********************************************************************/
/* � 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Alpha/Configuration.h"

#include <Windows.h>

Configuration::Configuration()
	: m_dataRoot()
	, m_rawBlenderPath()
	, m_rawShadersPath()
	, m_gameResolutionWidth(1920)	// default resolution is FHD
	, m_gameResolutionHeight(1080)	// default resolution is FHD
{}

Configuration::~Configuration()
{}

bool Configuration::Load(const std::string& binPath, const std::string& filename)
{
	m_binPath = binPath;

	const int BUFFER_SIZE = 256;
	char buffer[BUFFER_SIZE] = { '\0' };

	GetPrivateProfileString("data", "root", "NULL", buffer, BUFFER_SIZE, filename.c_str());
	m_dataRoot = buffer;

	GetPrivateProfileString("raw", "blender", "NULL", buffer, BUFFER_SIZE, filename.c_str());
	m_rawBlenderPath = buffer;

	GetPrivateProfileString("raw", "shaders", "NULL", buffer, BUFFER_SIZE, filename.c_str());
	m_rawShadersPath = buffer;

	GetPrivateProfileString("editors", "scripts", "NULL", buffer, BUFFER_SIZE, filename.c_str());
	m_editorsScriptsPath = buffer;

	GetPrivateProfileString("shader_editor", "shader_compiler", "NULL", buffer, BUFFER_SIZE, filename.c_str());
	m_shaderCompiler = buffer;

	GetPrivateProfileString("mesh_editor", "blender", "NULL", buffer, BUFFER_SIZE, filename.c_str());
	m_blender = buffer;

	GetPrivateProfileInt("game", "resolution_width", m_gameResolutionWidth, filename.c_str());
	GetPrivateProfileInt("game", "resolution_height", m_gameResolutionHeight, filename.c_str());

	return true;
}
