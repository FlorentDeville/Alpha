/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/RawDb.h"

#include <fstream>

namespace Editors
{
	void LoadRawDb(const std::string& rawDbFilename, RawDb& rawDb)
	{
		std::ifstream file(rawDbFilename);

		std::string line;
		while (std::getline(file, line))
		{
			//parse a single line
			size_t id;
			const int REL_PATH_MAX_LENGTH = 255;
			char relPath[REL_PATH_MAX_LENGTH] = { '\0' };
			sscanf_s(line.c_str(), "%[^,],%zu", relPath, REL_PATH_MAX_LENGTH, &id);

			//convert type to AssetType
			rawDb[Systems::AssetId(id)] = relPath;
		}
	}
}