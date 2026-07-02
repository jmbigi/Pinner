#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
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

static void WriteLog(const wchar_t *path, const wchar_t *timeStr, const wchar_t *context,
    const wchar_t *appPath, const wchar_t *cmdLine, DWORD errCode, const wchar_t *errMsg)
{
    FILE *f = _wfopen(path, L"a, ccs=UTF-8");
    if (!f) return;
    fwprintf(f, L"[%ls] %ls\n", timeStr, context);
    fwprintf(f, L"  App : %ls\n", appPath);
    fwprintf(f, L"  Cmd : %ls\n", cmdLine);
    fwprintf(f, L"  Err : %lu - %ls\n", errCode, errMsg);
    fwprintf(f, L"\n");
    fclose(f);
}

static void WriteLogs(const wchar_t *context, const wchar_t *appPath, const wchar_t *cmdLine, DWORD errCode, const wchar_t *errMsg)
{
    wchar_t timeStr[64] = L"";
    SYSTEMTIME st;
    GetLocalTime(&st);
    wsprintfW(timeStr, L"%04d-%02d-%02d %02d:%02d:%02d",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    wchar_t exePath[MAX_PATH], exeName[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    wchar_t *slash = wcsrchr(exePath, L'\\');
    wcscpy(exeName, slash ? slash + 1 : exePath);
    wchar_t *dot = wcsrchr(exeName, L'.');
    if (dot) *dot = 0;

    wchar_t logNear[MAX_PATH];
    wsprintfW(logNear, L"%ls.log", exePath);
    WriteLog(logNear, timeStr, context, appPath, cmdLine, errCode, errMsg);

    wchar_t appData[MAX_PATH];
    if (GetEnvironmentVariableW(L"APPDATA", appData, MAX_PATH) > 0) {
        wchar_t adPinner[MAX_PATH], adLog[MAX_PATH];
        wsprintfW(adPinner, L"%ls\\Pinner", appData);
        CreateDirectoryW(adPinner, NULL);
        wsprintfW(adLog, L"%ls\\logs", adPinner);
        CreateDirectoryW(adLog, NULL);
        wsprintfW(adLog, L"%ls\\logs\\%ls.log", adPinner, exeName);
        WriteLog(adLog, timeStr, context, appPath, cmdLine, errCode, errMsg);
    }

    wchar_t temp[MAX_PATH];
    if (GetEnvironmentVariableW(L"TEMP", temp, MAX_PATH) > 0) {
        wchar_t tmpDir[MAX_PATH], tmpLog[MAX_PATH];
        wsprintfW(tmpDir, L"%ls\\Pinner", temp);
        CreateDirectoryW(tmpDir, NULL);
        wsprintfW(tmpLog, L"%ls\\%ls.log", tmpDir, exeName);
        WriteLog(tmpLog, timeStr, context, appPath, cmdLine, errCode, errMsg);
    }
}

static void LogError(const wchar_t *context, const wchar_t *appPath, const wchar_t *cmdLine, DWORD errCode)
{
    wchar_t errMsg[1024] = L"";
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        errMsg, 1024, NULL);
    int len = wcslen(errMsg);
    while (len > 0 && (errMsg[len-1] == L'\r' || errMsg[len-1] == L'\n' || errMsg[len-1] == L' '))
        errMsg[--len] = 0;
    WriteLogs(context, appPath, cmdLine, errCode, errMsg);
}

static void LogInfo(const wchar_t *context, const wchar_t *appPath, const wchar_t *cmdLine)
{
    WriteLogs(context, appPath, cmdLine, 0, L"");
}

static void ShowError(const wchar_t *title, const wchar_t *appPath, const wchar_t *cmdLine, DWORD errCode)
{
    wchar_t errMsg[1024] = L"";
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        errMsg, 1024, NULL);
    int len = wcslen(errMsg);
    while (len > 0 && (errMsg[len-1] == L'\r' || errMsg[len-1] == L'\n' || errMsg[len-1] == L' '))
        errMsg[--len] = 0;

    wchar_t msg[4096];
    wsprintfW(msg,
        L"%ls\n\n"
        L"Comando: %ls\n"
        L"Programa: %ls\n\n"
        L"Error %lu: %ls"
#if FLAG_CONSOLE == 0
        L"\n\nSUGERENCIA: Prueba a generar el launcher con --console"
#endif
        ,
        title, cmdLine, appPath, errCode, errMsg);

    WriteLogs(title, appPath, cmdLine, errCode, msg);
    MessageBoxW(NULL, msg, APP_NAME, MB_ICONERROR | MB_OK);
}

static int ResolvePath(const wchar_t *name, wchar_t *fullPath, int maxLen)
{
    if (wcschr(name, L'\\') || wcschr(name, L'/') || wcsstr(name, L":") != NULL) {
        wcscpy(fullPath, name);
        return GetFileAttributesW(fullPath) != INVALID_FILE_ATTRIBUTES;
    }
    wchar_t *envPaths = NULL;
    DWORD envLen = GetEnvironmentVariableW(L"PATH", NULL, 0);
    if (envLen > 0) {
        envPaths = (wchar_t*)malloc(envLen * sizeof(wchar_t));
        if (envPaths) {
            GetEnvironmentVariableW(L"PATH", envPaths, envLen);
            wchar_t *ctx = NULL;
            wchar_t *tok = wcstok_s(envPaths, L";", &ctx);
            while (tok) {
                wsprintfW(fullPath, L"%ls\\%ls", tok, name);
                if (GetFileAttributesW(fullPath) != INVALID_FILE_ATTRIBUTES) {
                    free(envPaths);
                    return 1;
                }
                tok = wcstok_s(NULL, L";", &ctx);
            }
            free(envPaths);
        }
    }
    // Fallback: check System32 directly
    wchar_t sys32[MAX_PATH];
    GetSystemDirectoryW(sys32, MAX_PATH);
    wsprintfW(fullPath, L"%ls\\%ls", sys32, name);
    if (GetFileAttributesW(fullPath) != INVALID_FILE_ATTRIBUTES) return 1;

    wsprintfW(fullPath, L"%ls\\WindowsPowerShell\\v1.0\\%ls", sys32, name);
    if (GetFileAttributesW(fullPath) != INVALID_FILE_ATTRIBUTES) return 1;

    wcscpy(fullPath, name);
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLineA, int nShow)
{
    (void)hInst; (void)hPrev; (void)nShow;

    wchar_t lpCmdLine[1024] = L"";
    MultiByteToWideChar(CP_UTF8, 0, lpCmdLineA, -1, lpCmdLine, 1024);

    wchar_t appPath[MAX_PATH] = L"";
    wchar_t progName[MAX_PATH] = L"";
    wchar_t args[4096] = L"";
    wchar_t workDir[MAX_PATH] = L"";
    wchar_t msg[4096];
    int isProgramMode = 0;

    int showCmd = FLAG_CONSOLE ? SW_SHOWNORMAL : SW_HIDE;

    if (wcslen(PROGRAM_PATH) > 0) {
        isProgramMode = 1;
        wcscpy(progName, PROGRAM_PATH);
        wchar_t resolved[MAX_PATH];
        if (ResolvePath(PROGRAM_PATH, resolved, MAX_PATH)) {
            wcscpy(appPath, resolved);
        }
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
                wsprintfW(msg, L"Script no encontrado.\nRuta buscada:\n%ls", SCRIPT_PATH);
                LogError(L"SCRIPT_NOT_FOUND", L"", L"", 0);
                MessageBoxW(NULL, msg, APP_NAME, MB_ICONERROR | MB_OK);
                return 1;
            }
        }
        wcscpy(progName, L"powershell.exe");
        wchar_t pwshFull[MAX_PATH];
        if (ResolvePath(L"powershell.exe", pwshFull, MAX_PATH)) {
            wcscpy(appPath, pwshFull);
        }
        wsprintfW(args, L"-ExecutionPolicy Bypass -NoProfile -WindowStyle %ls -File \"%ls\"",
            FLAG_CONSOLE ? L"Normal" : L"Hidden",
            scriptPath);
        if (wcslen(lpCmdLine) > 0) {
            wcscat(args, L" ");
            wcscat(args, lpCmdLine);
        }
    } else {
        LogError(L"NO_PROGRAM_OR_SCRIPT", L"", L"", 0);
        MessageBoxW(NULL, L"No se configuro programa ni script.\n\nUsa --program o --script al generar el launcher.",
            APP_NAME, MB_ICONERROR | MB_OK);
        return 1;
    }

    LogInfo(L"STARTUP", progName, lpCmdLine);

    wchar_t displayPath[MAX_PATH];
    wcscpy(displayPath, appPath[0] ? appPath : progName);

    if (FLAG_ADMIN) {
        SHELLEXECUTEINFOW sei;
        ZeroMemory(&sei, sizeof(sei));
        sei.cbSize = sizeof(sei);
        sei.lpVerb = L"runas";
        sei.lpFile = displayPath;
        sei.lpParameters = args;
        sei.lpDirectory = wcslen(workDir) > 0 ? workDir : NULL;
        sei.nShow = showCmd;
        if (!ShellExecuteExW(&sei)) {
            DWORD err = GetLastError();
            ShowError(L"Error al ejecutar como administrador", displayPath, args, err);
            return 1;
        }
        LogInfo(L"ADMIN_LAUNCH_OK", displayPath, args);
        if (sei.hProcess) {
            WaitForSingleObject(sei.hProcess, INFINITE);
            DWORD exitCode = 0;
            GetExitCodeProcess(sei.hProcess, &exitCode);
            wchar_t exitCtx[128];
            wsprintfW(exitCtx, L"EXIT_CODE: %lu", exitCode);
            LogInfo(exitCtx, displayPath, args);
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
    wsprintfW(cmdLine, L"\"%ls\" %ls", progName, args);

    DWORD dwFlags = FLAG_CONSOLE ? 0 : CREATE_NO_WINDOW;

    if (!CreateProcessW(appPath[0] ? appPath : NULL, cmdLine, NULL, NULL, FALSE, dwFlags, NULL,
        wcslen(workDir) > 0 ? workDir : NULL, &si, &pi))
    {
        DWORD err = GetLastError();
        ShowError(L"Error al lanzar el programa", displayPath, cmdLine, err);
        return 1;
    }

    LogInfo(L"LAUNCH_OK", displayPath, cmdLine);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
