#include <iostream>
#include <windows.h>
#include <filesystem>
#include <shlobj.h>

#define MAXMODULE 50

typedef void (__cdecl *cfunc)(const char*);
typedef void (__cdecl *cwfunc)(const wchar_t*);

void GetFolder(bool bOut, wchar_t* wcFolderName)
{
	BROWSEINFO bInfo;
	bInfo.hwndOwner = NULL;
	bInfo.lpfn = NULL;
	bInfo.lParam = NULL;
	bInfo.lpszTitle = bOut ? L"Please select OutFolder" : L"Please select InFolder";
	bInfo.pidlRoot = NULL;
	bInfo.ulFlags = BIF_USENEWUI | BIF_VALIDATE;
	bInfo.pszDisplayName = NULL;//foldername;

	LPITEMIDLIST pidl;

	if ((pidl = SHBrowseForFolder(&bInfo)) != NULL) 
	{
		SHGetPathFromIDList(pidl, wcFolderName);
	}
}

void CopyAllFilesFromFolder(const std::filesystem::path& pathFromCopy, const std::filesystem::path& pathToCopy, int level)
{
	if (std::filesystem::exists(pathFromCopy) && std::filesystem::is_directory(pathFromCopy))
	{
		for (const auto& entry : std::filesystem::directory_iterator(pathFromCopy))
		{
			if (std::filesystem::is_directory(entry.status()))
			{
				CopyAllFilesFromFolder(entry, pathToCopy, level + 1);
			}
			else if (std::filesystem::is_regular_file(entry.status()))
			{
				// Copy file
				std::filesystem::path finalFilePath = pathToCopy / entry.path().filename();
				std::filesystem::remove(finalFilePath);
				std::filesystem::copy_file(entry.path(), finalFilePath);
			}
		}
	}
}

int main()
{
	cwfunc SetLogFileName;
	cfunc SetLogFormat;
	cfunc LogPrintError;
	cfunc LogPrintDebug;
	cfunc LogPrintTrace;

	// Get a handle to the DLL module.
	HINSTANCE hinstLib = LoadLibrary(L"logger.dll");
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

	// If the handle is valid, try to get the function address.
	if (hinstLib != NULL)
	{
		SetLogFileName = (cwfunc)GetProcAddress(hinstLib, "SetLoggerFileName");
		SetLogFormat = (cfunc)GetProcAddress(hinstLib, "SetLoggerFormat");
		LogPrintError = (cfunc)GetProcAddress(hinstLib, "LoggerPrintError");
		LogPrintDebug = (cfunc)GetProcAddress(hinstLib, "LoggerPrintDebug");
		LogPrintTrace = (cfunc)GetProcAddress(hinstLib, "LoggerPrintTrace");

		// If the function address is valid, call the function.

		if ((SetLogFileName != NULL) && (SetLogFormat != NULL) && (LogPrintError != NULL)
			&& (LogPrintDebug != NULL) && (LogPrintTrace != NULL))
		{
			fRunTimeLinkSuccess = TRUE;

			SetLogFormat("\n%%time%% %data%\t*prior*\t**message**");

			// Get folder paths
			wchar_t wcOutFolder[MAX_PATH];
			GetFolder(true, wcOutFolder);
			wchar_t wcInFolder[MAX_PATH];
			GetFolder(false, wcInFolder);

			std::filesystem::path outPath(wcOutFolder);
			std::filesystem::path inPath(wcInFolder);
			CopyAllFilesFromFolder(outPath, inPath, 0);

			// Write in all copied files message from logger
			for (const auto& entry : std::filesystem::directory_iterator(inPath))
			{
				if (std::filesystem::is_regular_file(entry.status()))
				{
					std::wstring wsFullPath = entry.path();
					SetLogFileName(wsFullPath.c_str());
					LogPrintTrace("File successfully copied");
				}
			}
		}
		// Free the DLL module.

		fFreeResult = FreeLibrary(hinstLib);
	}

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess)
		MessageBox(nullptr, L"Unable to call the DLL function.", L"Error", MB_OK | MB_ICONERROR);

    return 0;
}
