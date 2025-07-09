/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Editors
{
	class ShaderAutoGen
	{
	public:
		static void WriteAllHeaderFiles(const std::string& folder);

	private:
		static void WriteCBufferPerFrameHeaderFile(const std::string& folder);

		static void WriteCBufferPerObjectHeaderFile(const std::string& folder);
		
		static void WriteCBufferLightsHeaderFile(const std::string& folder);

		static void WriteBaseTypesHeaderFile(const std::string& folder);
	};
}
