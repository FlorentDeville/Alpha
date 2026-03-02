/********************************************************************************/
/* Copyright (C) 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include <string>
#include <vector>

namespace Os
{
	bool OpenFileDialog(std::string& filename, const std::string& extension = "");

	bool SaveFileDialog(std::string& outFilename, const std::vector<std::string>& extension, const std::string& filename = "");

	bool SelectFolderDialog(const std::string& title, std::string& folder);
}
