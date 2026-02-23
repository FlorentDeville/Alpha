/********************************************************************/
/* © 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "Systems/Assets/AssetObjects/Texture/TextureAsset.h"

namespace Importer
{
	class TextureImporter
	{
	public:
		bool Import(const std::string& sourceFilename, Systems::TextureAsset* pTexture);

	private:
	};
}
