typedef struct {
    const wchar_t *name;
    const wchar_t *desc;
    const wchar_t *program;
    const wchar_t *args;
    const wchar_t *workdir;
    const wchar_t *iconFile;
    int admin;
    int console;
} Preset;

Preset presets[] = {
    {
        L"Chrome Privado",
        L"Chrome en modo incognito, sin historial ni cookies",
        L"chrome.exe", L"-incognito", L"",
        L"chrome", 0, 0
    },
    {
        L"Chrome Perfil Trabajo",
        L"Chrome con perfil de trabajo separado",
        L"chrome.exe", L"--profile-directory=\"Profile 1\"", L"",
        L"chrome", 0, 0
    },
    {
        L"Edge Privado",
        L"Microsoft Edge en modo InPrivate",
        L"msedge.exe", L"-inprivate", L"",
        L"microsoft-edge", 0, 0
    },
    {
        L"Firefox Privado",
        L"Firefox en navegacion privada",
        L"firefox.exe", L"-private-window", L"",
        L"firefox", 0, 0
    },
    {
        L"Terminal en carpeta",
        L"PowerShell en una carpeta especifica (configurar --workdir)",
        L"powershell.exe", L"-NoExit -Command Set-Location '.'", L"",
        L"powershell", 0, 1
    },
    {
        L"Terminal Admin",
        L"PowerShell como administrador",
        L"powershell.exe", L"-NoExit", L"",
        L"powershell", 1, 1
    },
    {
        L"Símbolo del sistema",
        L"CMD clasico",
        L"cmd.exe", L"/k", L"",
        L"terminal", 0, 1
    },
    {
        L"Abrir URL",
        L"Abre una URL en el navegador predeterminado",
        L"cmd.exe", L"/c start", L"",
        L"chrome", 0, 0
    },
    {
        L"Bloc de notas",
        L"Notepad clasico",
        L"notepad.exe", L"", L"",
        L"notepad", 0, 0
    },
    {
        L"Explorador",
        L"Abrir explorador de archivos en carpeta (configurar --workdir)",
        L"explorer.exe", L"", L"",
        L"windows", 0, 0
    },
    {
        L"VS Code",
        L"Abrir Visual Studio Code en carpeta (configurar --workdir)",
        L"code.exe", L".", L"",
        L"visual-studio-code", 0, 1
    },
    {
        L"Python Script",
        L"Ejecutar un script Python (pasar ruta como argumento al anclar)",
        L"python.exe", L"", L"",
        L"python", 0, 1
    },
    {NULL, NULL, NULL, NULL, NULL, NULL, 0, 0}
};
