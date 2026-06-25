#define WIN32_LEAN_AND_MEAN
#define _WIN32_IE 0x0600
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "presets.h"

#define MAX_VAL 1024

typedef struct {
    wchar_t name[MAX_VAL];
    wchar_t script[MAX_VAL];
    wchar_t program[MAX_VAL];
    wchar_t args[MAX_VAL];
    wchar_t workdir[MAX_VAL];
    wchar_t icon[MAX_VAL];
    wchar_t output[MAX_VAL];
    wchar_t preset[MAX_VAL];
    int admin;
    int console;
    int presetApplied;
} Config;

void show_help(void)
{
    printf("PINNER - Personaliza tu acceso a programas y scripts\n");
    printf("Genera .exe launcher para anclar al taskbar.\n\n");
    printf("USO:\n");
    printf("  pinmaker -n \"MiApp\" -s \"C:\\ruta\\script.ps1\"\n");
    printf("  pinmaker -n \"Chrome\" -p \"chrome.exe\" -a \"-incognito\"\n");
    printf("  pinmaker -n \"Terminal\" -p \"powershell.exe\" -a \"-NoExit -Command Set-Location 'C:\\Proyecto'\" --console\n\n");
    printf("OPCIONES:\n");
    printf("  --name,-n     Nombre del acceso\n");
    printf("  --script,-s   Ruta al script .ps1\n");
    printf("  --program,-p  Ruta al programa .exe\n");
    printf("  --args,-a     Argumentos del programa\n");
    printf("  --workdir,-d  Carpeta de trabajo\n");
    printf("  --icon,-i     Ruta al .ico (default: P)\n");
    printf("  --preset      Nombre del preset (ver --preset list)\n");
    printf("  --admin       Ejecutar como administrador\n");
    printf("  --console     Mostrar ventana de consola\n");
    printf("  --output,-o   Carpeta salida (default: .\\exes)\n");
    printf("  --help,-h     Esta ayuda\n\n");
    printf("PRESETS:\n");
    printf("  pinmaker --preset list      Lista todos los presets disponibles\n");
    printf("  pinmaker -n \"Mi Chrome\" --preset \"Chrome Privado\"\n\n");
    printf("EJEMPLOS:\n");
    printf("  pinmaker -n \"PS Trabajo\" -s \"C:\\scripts\\terminal.ps1\"\n");
    printf("  pinmaker -n \"Chrome Privado\" -p \"chrome.exe\" -a \"-incognito\" -i chrome.ico\n");
    printf("  pinmaker -n \"VPN\" -p \"C:\\VPN\\client.exe\" --admin\n");
    printf("  pinmaker -n \"Python\" -p \"python.exe\" -a \"app.py\" -d \"C:\\Proyecto\" --console\n\n");
    printf("Luego anclar al taskbar: clic derecho en .exe -> Anclar a la barra de tareas\n");
}

void list_presets(void)
{
    wprintf(L"PRESETS DISPONIBLES:\n");
    wprintf(L"%-24ls %ls\n", L"NOMBRE", L"DESCRIPCION");
    wprintf(L"%-24ls %ls\n", L"------", L"-----------");
    for (int i = 0; presets[i].name; i++) {
        wprintf(L"%-24ls %ls\n", presets[i].name, presets[i].desc);
    }
    wprintf(L"\nUso: pinmaker -n \"Mi App\" --preset \"Chrome Privado\"\n");
}

int apply_preset(Config *cfg, const wchar_t *presetName)
{
    for (int i = 0; presets[i].name; i++) {
        if (_wcsicmp(presetName, presets[i].name) == 0) {
            wcscpy(cfg->program, presets[i].program);
            wcscpy(cfg->args, presets[i].args);
            wcscpy(cfg->workdir, presets[i].workdir);
            cfg->admin = presets[i].admin;
            cfg->console = presets[i].console;
            if (wcslen(presets[i].iconFile) > 0) {
                // Look for icon in icons/ico/ relative to pinmaker location
                wchar_t modPath[MAX_PATH], drive[MAX_PATH], dir[MAX_PATH];
                GetModuleFileNameW(NULL, modPath, MAX_PATH);
                _wsplitpath(modPath, drive, dir, NULL, NULL);
                wsprintfW(cfg->icon, L"%s%licons%lico%s%s.ico", drive, dir, L"\\", L"\\", presets[i].iconFile);
                if (GetFileAttributesW(cfg->icon) == INVALID_FILE_ATTRIBUTES) {
                    cfg->icon[0] = 0;
                }
            }
            cfg->presetApplied = 1;
            return 1;
        }
    }
    return 0;
}

void to_wide(const char *src, wchar_t *dst, int max)
{
    MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, max);
}

int parse_args(int argc, char *argv[], Config *cfg)
{
    int i;
    wcscpy(cfg->name, L"");
    wcscpy(cfg->script, L"");
    wcscpy(cfg->program, L"");
    wcscpy(cfg->args, L"");
    wcscpy(cfg->workdir, L"");
    wcscpy(cfg->icon, L"");
    wcscpy(cfg->preset, L"");
    wcscpy(cfg->output, L".\\exes");
    cfg->admin = 0;
    cfg->console = 0;
    cfg->presetApplied = 0;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            show_help();
            return 0;
        }
        else if (strcmp(argv[i], "--name") == 0 || strcmp(argv[i], "-n") == 0) {
            if (++i < argc) to_wide(argv[i], cfg->name, MAX_VAL);
        }
        else if (strcmp(argv[i], "--script") == 0 || strcmp(argv[i], "-s") == 0) {
            if (++i < argc) to_wide(argv[i], cfg->script, MAX_VAL);
        }
        else if (strcmp(argv[i], "--program") == 0 || strcmp(argv[i], "-p") == 0) {
            if (++i < argc) to_wide(argv[i], cfg->program, MAX_VAL);
        }
        else if (strcmp(argv[i], "--args") == 0 || strcmp(argv[i], "-a") == 0) {
            if (++i < argc) to_wide(argv[i], cfg->args, MAX_VAL);
        }
        else if (strcmp(argv[i], "--workdir") == 0 || strcmp(argv[i], "-d") == 0) {
            if (++i < argc) to_wide(argv[i], cfg->workdir, MAX_VAL);
        }
        else if (strcmp(argv[i], "--icon") == 0 || strcmp(argv[i], "-i") == 0) {
            if (++i < argc) to_wide(argv[i], cfg->icon, MAX_VAL);
        }
        else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) {
            if (++i < argc) to_wide(argv[i], cfg->output, MAX_VAL);
        }
        else if (strcmp(argv[i], "--preset") == 0) {
            if (++i < argc) to_wide(argv[i], cfg->preset, MAX_VAL);
        }
        else if (strcmp(argv[i], "--admin") == 0) {
            cfg->admin = 1;
        }
        else if (strcmp(argv[i], "--console") == 0) {
            cfg->console = 1;
        }
    }
    return 1;
}

void escape_c_string(const wchar_t *src, wchar_t *dst, int max)
{
    int j, k;
    for (j = 0, k = 0; src[j] && k < max - 3; j++) {
        if (src[j] == L'\\') { dst[k++] = L'\\'; dst[k++] = L'\\'; }
        else if (src[j] == L'"') { dst[k++] = L'\\'; dst[k++] = L'"'; }
        else { dst[k++] = src[j]; }
    }
    dst[k] = 0;
}

wchar_t* find_template(wchar_t *buf, int bufSize)
{
    wchar_t modPath[MAX_PATH];
    GetModuleFileNameW(NULL, modPath, MAX_PATH);
    wchar_t drive[MAX_PATH], dir[MAX_PATH];
    _wsplitpath(modPath, drive, dir, NULL, NULL);

    swprintf(buf, bufSize, L"%s%ssrc%slauncher_template.c", drive, dir, L"\\");
    if (GetFileAttributesW(buf) != INVALID_FILE_ATTRIBUTES) return buf;

    wcscpy(buf, L"src\\launcher_template.c");
    if (GetFileAttributesW(buf) != INVALID_FILE_ATTRIBUTES) return buf;

    swprintf(buf, bufSize, L"launcher_template.c");
    if (GetFileAttributesW(buf) != INVALID_FILE_ATTRIBUTES) return buf;

    return NULL;
}

int has_define_line(const char *line)
{
    const char *defines[] = {
        "#define APP_NAME", "#define APP_DESC",
        "#define SCRIPT_PATH",
        "#define PROGRAM_PATH", "#define PROGRAM_ARGS", "#define WORK_DIR",
        "#define FLAG_ADMIN", "#define FLAG_CONSOLE",
        NULL
    };
    for (int i = 0; defines[i]; i++) {
        if (strstr(line, defines[i])) return 1;
    }
    return 0;
}

int build_launcher(Config *cfg)
{
    wchar_t templatePath[MAX_PATH];
    wchar_t tempDir[MAX_PATH];
    wchar_t tempC[MAX_PATH];
    wchar_t tempRc[MAX_PATH];
    wchar_t tempRes[MAX_PATH];
    wchar_t outExe[MAX_PATH];
    wchar_t cmd[MAX_PATH * 4];
    char cmdA[MAX_PATH * 8];
    FILE *f;
    int hasIcon = (wcslen(cfg->icon) > 0 && GetFileAttributesW(cfg->icon) != INVALID_FILE_ATTRIBUTES);
    int hasScript = (wcslen(cfg->script) > 0);
    int hasProgram = (wcslen(cfg->program) > 0);

    if (hasScript && hasProgram) {
        printf("ERROR: Usa --script O --program, no ambos.\n");
        return 0;
    }
    if (!hasScript && !hasProgram) {
        printf("ERROR: Necesitas --script o --program.\n");
        return 0;
    }

    GetTempPathW(MAX_PATH, tempDir);
    swprintf(tempC, MAX_PATH, L"%spinner_XXXX.c", tempDir);
    swprintf(tempRc, MAX_PATH, L"%spinner_XXXX.rc", tempDir);
    swprintf(tempRes, MAX_PATH, L"%spinner_XXXX.res", tempDir);
    swprintf(outExe, MAX_PATH, L"%s\\%s.exe", cfg->output, cfg->name);

    wchar_t *tmpl = find_template(templatePath, MAX_PATH);
    if (!tmpl) {
        printf("ERROR: No se encuentra launcher_template.c\n");
        return 0;
    }

    FILE *fin = _wfopen(tmpl, L"rb");
    if (!fin) { printf("ERROR: No se puede leer %ls\n", tmpl); return 0; }

    f = _wfopen(tempC, L"wb");
    if (!f) { fclose(fin); printf("ERROR: No se puede escribir temp\n"); return 0; }

    wchar_t escapedName[MAX_PATH * 2];
    escape_c_string(cfg->name, escapedName, MAX_PATH * 2);

    fprintf(f, "/* GENERADO POR PINNER */\n");
    fprintf(f, "#define APP_NAME L\"%ls\"\n", cfg->name);
    fprintf(f, "#define APP_DESC L\"%ls - Pinner Launcher\"\n", escapedName);

    if (hasScript) {
        wchar_t scriptAbs[MAX_PATH];
        if (wcschr(cfg->script, L':') || cfg->script[0] == L'\\') {
            wcscpy(scriptAbs, cfg->script);
        } else {
            wchar_t cwd[MAX_PATH];
            GetCurrentDirectoryW(MAX_PATH, cwd);
            swprintf(scriptAbs, MAX_PATH, L"%s\\%s", cwd, cfg->script);
        }
        wchar_t escapedScript[MAX_PATH * 2];
        escape_c_string(scriptAbs, escapedScript, MAX_PATH * 2);

        fprintf(f, "#define SCRIPT_PATH L\"%ls\"\n", escapedScript);
        fprintf(f, "#define PROGRAM_PATH L\"\"\n");
        fprintf(f, "#define PROGRAM_ARGS L\"\"\n");
        fprintf(f, "#define WORK_DIR L\"\"\n");
    } else {
        wchar_t programAbs[MAX_PATH];
        if (wcschr(cfg->program, L':') || cfg->program[0] == L'\\') {
            wcscpy(programAbs, cfg->program);
        } else {
            wchar_t cwd[MAX_PATH];
            GetCurrentDirectoryW(MAX_PATH, cwd);
            swprintf(programAbs, MAX_PATH, L"%s\\%s", cwd, cfg->program);
        }
        wchar_t escapedProgram[MAX_PATH * 2];
        wchar_t escapedArgs[MAX_PATH * 2];
        wchar_t escapedWorkDir[MAX_PATH * 2];
        escape_c_string(programAbs, escapedProgram, MAX_PATH * 2);
        escape_c_string(cfg->args, escapedArgs, MAX_PATH * 2);
        escape_c_string(cfg->workdir, escapedWorkDir, MAX_PATH * 2);

        fprintf(f, "#define SCRIPT_PATH L\"\"\n");
        fprintf(f, "#define PROGRAM_PATH L\"%ls\"\n", escapedProgram);
        fprintf(f, "#define PROGRAM_ARGS L\"%ls\"\n", escapedArgs);
        fprintf(f, "#define WORK_DIR L\"%ls\"\n", escapedWorkDir);
    }

    fprintf(f, "#define FLAG_ADMIN %d\n", cfg->admin ? 1 : 0);
    fprintf(f, "#define FLAG_CONSOLE %d\n", cfg->console ? 1 : 0);

    char line[1024];
    while (fgets(line, sizeof(line), fin)) {
        if (has_define_line(line)) continue;
        fputs(line, f);
    }
    fclose(fin);
    fclose(f);

    if (hasIcon) {
        f = _wfopen(tempRc, L"wb");
        if (f) {
            wchar_t iconFwd[MAX_PATH];
            int j;
            for (j = 0; cfg->icon[j] && j < MAX_PATH - 1; j++)
                iconFwd[j] = (cfg->icon[j] == L'\\') ? L'/' : cfg->icon[j];
            iconFwd[j] = 0;

            wchar_t nameFwd[MAX_PATH];
            for (j = 0; cfg->name[j] && j < MAX_PATH - 1; j++)
                nameFwd[j] = (cfg->name[j] == L'\\') ? L'/' : cfg->name[j];
            nameFwd[j] = 0;

            fprintf(f, "1 ICON \"%ls\"\n", iconFwd);
            fprintf(f, "1 VERSIONINFO\n");
            fprintf(f, "  FILEVERSION 1,0,0,0\n");
            fprintf(f, "  PRODUCTVERSION 1,0,0,0\n");
            fprintf(f, "  BEGIN\n");
            fprintf(f, "    BLOCK \"StringFileInfo\"\n");
            fprintf(f, "    BEGIN\n");
            fprintf(f, "      BLOCK \"040904B0\"\n");
            fprintf(f, "      BEGIN\n");
            fprintf(f, "        VALUE \"FileDescription\", \"%ls\"\n", nameFwd);
            fprintf(f, "        VALUE \"ProductName\", \"Pinner Launcher\"\n");
            fprintf(f, "      END\n");
            fprintf(f, "    END\n");
            fprintf(f, "    BLOCK \"VarFileInfo\"\n");
            fprintf(f, "    BEGIN\n");
            fprintf(f, "      VALUE \"Translation\", 0x409, 1200\n");
            fprintf(f, "    END\n");
            fprintf(f, "  END\n");
            fclose(f);
        }
    }

    CreateDirectoryW(cfg->output, NULL);

    const wchar_t *modeLabel = hasProgram ? L"programa" : L"script";
    printf("Compilando: %ls (%ls)\n", cfg->name, modeLabel);

    int ret = -1;
    if (hasIcon) {
        swprintf(cmd, MAX_PATH * 4, L"windres -i \"%ls\" -O coff -o \"%ls\"", tempRc, tempRes);
        WideCharToMultiByte(CP_ACP, 0, cmd, -1, cmdA, sizeof(cmdA), NULL, NULL);
        ret = system(cmdA);
        if (ret != 0) {
            printf("ERROR: windres fallo al compilar icono\n");
            _wremove(tempC); _wremove(tempRc); _wremove(tempRes);
            return 0;
        }
        swprintf(cmd, MAX_PATH * 4, L"gcc \"%ls\" \"%ls\" -o \"%ls\" -mwindows -Os -s -lshlwapi",
            tempC, tempRes, outExe);
    } else {
        swprintf(cmd, MAX_PATH * 4, L"gcc \"%ls\" -o \"%ls\" -mwindows -Os -s -lshlwapi",
            tempC, outExe);
    }
    WideCharToMultiByte(CP_ACP, 0, cmd, -1, cmdA, sizeof(cmdA), NULL, NULL);
    ret = system(cmdA);

    _wremove(tempC);
    _wremove(tempRc);
    _wremove(tempRes);

    if (ret == 0) {
        printf("OK: %ls\n", outExe);
        printf("\nAHORA:\n");
        printf("  1. Clic derecho en \"%ls.exe\" -> Anclar a la barra de tareas\n", cfg->name);
        printf("  2. Arrastrar a la posicion deseada\n");
        return 1;
    } else {
        printf("ERROR: Compilacion fallo (codigo %d)\n", ret);
        printf("Asegurate de tener MinGW-w64 (gcc) en PATH.\n");
        _wremove(outExe);
        return 0;
    }
}

int main(int argc, char *argv[])
{
    Config cfg;

    if (argc < 2) {
        show_help();
        return 0;
    }

    if (!parse_args(argc, argv, &cfg)) return 0;

    // Handle --preset list
    if (wcslen(cfg.preset) > 0 && _wcsicmp(cfg.preset, L"list") == 0) {
        list_presets();
        return 0;
    }

    // Apply preset if specified
    if (wcslen(cfg.preset) > 0) {
        if (!apply_preset(&cfg, cfg.preset)) {
            wprintf(L"ERROR: Preset \"%ls\" no encontrado.\n", cfg.preset);
            printf("Usa: pinmaker --preset list\n");
            return 1;
        }
        wprintf(L"Preset aplicado: %ls\n", cfg.preset);
    }

    if (wcslen(cfg.name) == 0) {
        printf("ERROR: --name es requerido\n");
        return 1;
    }

    return build_launcher(&cfg) ? 0 : 1;
}
