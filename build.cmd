@echo off
echo === PINNER - Build ===
echo.

echo [1/4] Generando icono por defecto...
powershell -NoProfile -ExecutionPolicy Bypass -File gen_icon.ps1

echo [2/4] Compilando pinmaker.exe...
gcc src\pinmaker.c -o pinmaker.exe -Os -s -lshlwapi || echo ERROR: gcc fallo && exit /b 1
echo  OK: pinmaker.exe

echo [3/4] Creando carpeta exes...
if not exist exes mkdir exes
echo  OK: exes\

echo [4/4] Generando Pinner Studio.exe...
pinmaker.exe -n "Pinner Studio" -s "src\pinner-studio.ps1" --console > nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo  OK: exes\Pinner Studio.exe
) else (
    echo  WARN: No se pudo generar Pinner Studio.exe
)

echo.
echo === BUILD OK ===
echo.
echo Launchers generados en .\exes\
echo.
echo Usar PINNER STUDIO (GUI):
echo   .\exes\Pinner Studio.exe
echo.
echo Usar PINMAKER (CLI):
echo   pinmaker -n "Mi App" -p "chrome.exe" -a "-incognito"
echo   pinmaker -n "Terminal" -p "powershell.exe" -a "-NoExit" --console
echo   pinmaker --preset list
echo.
pause
