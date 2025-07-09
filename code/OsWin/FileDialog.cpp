/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "OsWin/FileDialog.h"

#include <Windows.h>
#include <shlobj.h>

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

    struct ScopeComInitialize
    {
        ScopeComInitialize()
        {
            m_res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        }

        ~ScopeComInitialize()
        {
            CoUninitialize();
        }

        HRESULT m_res;
    };

    struct ScopeFileOpenDialog
    {
        ScopeFileOpenDialog()
        {
            m_pFileOpen = nullptr;
            m_res = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&m_pFileOpen));
        }

        ~ScopeFileOpenDialog()
        {
            if(SUCCEEDED(m_res) && m_pFileOpen)
                m_pFileOpen->Release();
        }

        IFileOpenDialog* m_pFileOpen;
        HRESULT m_res;
    };

    bool SelectFolderDialog(const std::string& title, std::string& folder)
    {
        ScopeComInitialize comJanitor;
        if (FAILED(comJanitor.m_res))
            return false;

        ScopeFileOpenDialog fileDialogJanitor;
        if (FAILED(fileDialogJanitor.m_res))
            return false;

        DWORD dwOptions;
        HRESULT hr = fileDialogJanitor.m_pFileOpen->GetOptions(&dwOptions);
        if (FAILED(hr))
            return false;

        std::wstring wideTitle(title.begin(), title.end());
        fileDialogJanitor.m_pFileOpen->SetTitle(wideTitle.c_str());

        hr = fileDialogJanitor.m_pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
        if (FAILED(hr))
            return false;

        hr = fileDialogJanitor.m_pFileOpen->Show(NULL);
        if (FAILED(hr))
            return false;

        IShellItem* pItem;
        hr = fileDialogJanitor.m_pFileOpen->GetResult(&pItem);
        if (FAILED(hr))
            return false;

        PWSTR pWideFolder;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pWideFolder);
        if (SUCCEEDED(hr)) 
        {
            const int BUFFER_SIZE = 260;
            char buffer[BUFFER_SIZE] = { '\0' };
            char defaultChar = ' ';
            WideCharToMultiByte(CP_ACP, 0, pWideFolder, -1, buffer, BUFFER_SIZE, &defaultChar, nullptr);
            folder = buffer;

            CoTaskMemFree(pWideFolder);
            pItem->Release();

            return true;
        }

        return false;
    }
}
