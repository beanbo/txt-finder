# txt-finder

A Windows console program from 2021 (Visual Studio project `SP_lab2`) that uses the
[Simple-logger](https://github.com/beanbo/Simple-logger) DLL.

> **What it actually does.** Despite the name, the program does not search text. It copies
> files from one folder to another and then **appends a log line to the end of every copied
> file**. Try it on test folders only.

## How it works (from `SP_lab2/SP_lab2.cpp`)

1. Loads `logger.dll` at run time (`LoadLibrary` / `GetProcAddress`). If the DLL or any of its
   functions is missing, it shows the message box *"Unable to call the DLL function."*
2. Shows two folder pickers: first **"Please select OutFolder"** (source), then
   **"Please select InFolder"** (destination).
3. Copies every file from the source folder and all its subfolders into the destination folder
   (flat, without recreating subfolders); a file with the same name in the destination is
   replaced.
4. For every file now in the destination folder it calls the logger with that file as the log
   file, so each file gets the line `<time> <date>	Trace	File successfully copied`
   appended to it.

## Stack

- C++17 (`std::filesystem`), WinAPI (`SHBrowseForFolder`, `LoadLibrary`, `MessageBox`)
- Visual Studio solution `SP_lab2.sln`, platform toolset v142 (Visual Studio 2019)
- A prebuilt `logger.dll` is committed in `SP_lab2/`

## Build and run

Open `SP_lab2.sln` in Visual Studio 2019 or newer, build, and run with `logger.dll` next to the
executable or in the working directory. Not verified here: the project needs Windows and MSVC.

`SP_lab2/log.txt` is a sample log from November 2021 left in the repository.

## Context

Laboratory work No. 2 of a system programming course (the project name `SP_lab2`), 2021.

## License

Public domain — see [LICENSE](LICENSE) (The Unlicense).
