/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <d3d12.h>

namespace Rendering
{
	class Shader
	{
	public:
		Shader(const std::string& path);
		~Shader();

		ID3DBlob* GetBlob() const;

	private:
		std::string m_path;
		ID3DBlob* m_pBlob;
	};
}
