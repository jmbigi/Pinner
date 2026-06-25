#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>

#ifndef APP_NAME
#  define APP_NAME L"Pinner Launcher"
#endif
#ifndef APP_DESC
#  define APP_DESC L"Pinner Launcher"
#endif

#ifndef SCRIPT_PATH
#  define SCRIPT_PATH L""
#endif

#ifndef PROGRAM_PATH
#  define PROGRAM_PATH L""
#endif
#ifndef PROGRAM_ARGS
#  define PROGRAM_ARGS L""
#endif
#ifndef WORK_DIR
#  define WORK_DIR L""
#endif

#ifndef FLAG_ADMIN
#  define FLAG_ADMIN 0
#endif
#ifndef FLAG_CONSOLE
#  define FLAG_CONSOLE 0
#endif

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLineA, int nShow)
{
    wchar_t lpCmdLine[1024] = L"";
    MultiByteToWideChar(CP_ACP, 0, lpCmdLineA, -1, lpCmdLine, 1024);

    wchar_t appPath[MAX_PATH] = L"";
    wchar_t args[4096] = L"";
    wchar_t workDir[MAX_PATH] = L"";
    wchar_t msg[4096];
    int isProgramMode = 0;

    int showCmd = FLAG_CONSOLE ? SW_SHOWNORMAL : SW_HIDE;

    if (wcslen(PROGRAM_PATH) > 0) {
        isProgramMode = 1;
        wcscpy(appPath, PROGRAM_PATH);
        wcscpy(args, PROGRAM_ARGS);
        wcscpy(workDir, WORK_DIR);
        if (wcslen(lpCmdLine) > 0) {
            if (wcslen(args) > 0) wcscat(args, L" ");
            wcscat(args, lpCmdLine);
        }
    } else if (wcslen(SCRIPT_PATH) > 0) {
        wchar_t scriptPath[MAX_PATH];
        wcscpy(scriptPath, SCRIPT_PATH);
        if (scriptPath[0] != L'\\' && wcsstr(scriptPath, L":") == NULL) {
            wchar_t exePath[MAX_PATH], drive[MAX_PATH], dir[MAX_PATH];
            GetModuleFileNameW(NULL, exePath, MAX_PATH);
            _wsplitpath(exePath, drive, dir, NULL, NULL);
            wsprintfW(scriptPath, L"%ls%ls%ls", drive, dir, SCRIPT_PATH);
        }
        if (GetFileAttributesW(scriptPath) == INVALID_FILE_ATTRIBUTES) {
            wchar_t exePath[MAX_PATH], drive[MAX_PATH], dir[MAX_PATH], name[MAX_PATH];
            GetModuleFileNameW(NULL, exePath, MAX_PATH);
            _wsplitpath(exePath, drive, dir, name, NULL);
            wsprintfW(scriptPath, L"%ls%ls%ls.ps1", drive, dir, name);
            if (GetFileAttributesW(scriptPath) == INVALID_FILE_ATTRIBUTES) {
                wsprintfW(msg, L"Script no encontrado.\n\nUsa --script con ruta valida al generar el launcher.\n\nBuscado:\n%ls", SCRIPT_PATH);
                MessageBoxW(NULL, msg, APP_NAME, MB_ICONERROR | MB_OK);
                return 1;
            }
        }
        wcscpy(appPath, L"powershell.exe");
        wsprintfW(args, L"-ExecutionPolicy Bypass -NoProfile -WindowStyle %ls -File \"%ls\"",
            FLAG_CONSOLE ? L"Normal" : L"Hidden", scriptPath);
        if (wcslen(lpCmdLine) > 0) {
            wcscat(args, L" ");
            wcscat(args, lpCmdLine);
        }
    } else {
        MessageBoxW(NULL, L"No se configuro programa ni script.\n\nUsa --program o --script al generar el launcher.",
            APP_NAME, MB_ICONERROR | MB_OK);
        return 1;
    }

    if (FLAG_ADMIN) {
        SHELLEXECUTEINFOW sei;
        ZeroMemory(&sei, sizeof(sei));
        sei.cbSize = sizeof(sei);
        sei.lpVerb = L"runas";
        sei.lpFile = appPath;
        sei.lpParameters = args;
        sei.lpDirectory = wcslen(workDir) > 0 ? workDir : NULL;
        sei.nShow = showCmd;
        if (!ShellExecuteExW(&sei)) {
            wsprintfW(msg, L"Error al ejecutar como administrador:\n%ls", appPath);
            MessageBoxW(NULL, msg, APP_NAME, MB_ICONERROR | MB_OK);
            return 1;
        }
        if (sei.hProcess) {
            WaitForSingleObject(sei.hProcess, INFINITE);
            CloseHandle(sei.hProcess);
        }
        return 0;
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = showCmd;
    ZeroMemory(&pi, sizeof(pi));

    wchar_t cmdLine[4096];
    wsprintfW(cmdLine, L"\"%ls\" %ls", appPath, args);

    DWORD dwFlags = FLAG_CONSOLE ? 0 : CREATE_NO_WINDOW;

    if (!CreateProcessW(NULL, cmdLine, NULL, NULL, FALSE, dwFlags, NULL,
        wcslen(workDir) > 0 ? workDir : NULL, &si, &pi))
    {
        wsprintfW(msg, L"Error al ejecutar:\n%ls\n\nComando:\n%ls", appPath, cmdLine);
        MessageBoxW(NULL, msg, APP_NAME, MB_ICONERROR | MB_OK);
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
