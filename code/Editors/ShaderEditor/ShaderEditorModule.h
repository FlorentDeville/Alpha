/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <string>

namespace Editors
{
	class ShaderEditorModule : public Core::Singleton<ShaderEditorModule>
	{
	public:

		bool NewShader(const std::string& virtualName);
	};
}
