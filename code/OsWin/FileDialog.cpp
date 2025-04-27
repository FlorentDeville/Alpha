/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "OsWin/FileDialog.h"

#include <Windows.h>


namespace Os
{
	bool OpenFileDialog(std::string& filename, const std::string& extension)
	{
        const int FILENAME_LENGTH = 260;
        char rawFilename[FILENAME_LENGTH] = { '\0' };

        OPENFILENAME ofn;       // common dialog box structure
        HWND hwnd = NULL;              // owner window is null

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = rawFilename;
        ofn.nMaxFile = FILENAME_LENGTH;
        ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        // Display the Open dialog box
        if (GetOpenFileName(&ofn) == TRUE)
        {
            filename = rawFilename;
            return true;
        }
        
        return false;
	}
}
