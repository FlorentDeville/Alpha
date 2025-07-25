/********************************************************************/
/* � 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

namespace Os
{
	bool OpenFileDialog(std::string& filename, const std::string& extension = "");

	bool SelectFolderDialog(const std::string& title, std::string& folder);
}
