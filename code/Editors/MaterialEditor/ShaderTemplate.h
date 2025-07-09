/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Editors
{
	class ShaderTemplate
	{
	public:
		static void WriteVertexShaderTemplate(const std::string& filename);

		static void WritePixelShaderTemplate(const std::string& filename);
	};
}
