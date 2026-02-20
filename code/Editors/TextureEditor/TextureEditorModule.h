/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"

#include <string>

namespace Editors
{
	class TextureEditorModule : public Core::Singleton<TextureEditorModule>
	{
	public:
		TextureEditorModule();
		~TextureEditorModule();

		void Init() override;
		void Shutdown() override;

		bool Import(const std::string& filename);
	};
}
