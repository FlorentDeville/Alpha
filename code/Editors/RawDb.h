/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <map>

#include "Systems/Assets/AssetId.h"

namespace Editors
{
	//RawDb is a map between a raw asset and an asset id. It tells you where is imported a raw asset.
	typedef std::map<Systems::AssetId, std::string> RawDb;

	void LoadRawDb(const std::string& rawDbFilename, RawDb& rawDb);
}
