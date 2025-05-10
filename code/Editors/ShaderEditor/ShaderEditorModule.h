/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include "Systems/Assets/NewAssetId.h"

#include <string>

namespace Editors
{
	class ShaderEditorModule : public Core::Singleton<ShaderEditorModule>
	{
	public:

		bool NewShader(const std::string& virtualName);
		bool SaveShader(Systems::NewAssetId id);
	};
}
