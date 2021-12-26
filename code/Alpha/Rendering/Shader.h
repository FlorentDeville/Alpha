/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <d3d12.h>

class Shader
{
public:
	Shader(const std::string& path);
	~Shader();

	ID3DBlob* GetBlob() const;

private:
	ID3DBlob* m_pBlob;
	std::string m_path;
};
