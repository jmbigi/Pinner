# Lecciones para el agente

## PRINCIPIOS ABSOLUTOS (no negociables)

### 0. CREER AL USUARIO SIEMPRE
- Si dice "sale un error" → el error EXISTE. Punto. No discutir, no asumir.
- Si dice "no funciona" → NO funciona. Preguntar qué exactamente vio, no contradecir.
- El usuario ve la GUI, el agente no. El usuario tiene la razón sobre lo que ve.

### 0b. NO ROMPER NADA
- No modificar código sin entender primero qué hace.
- Leer el archivo completo antes de editar.
- Si no se entiende el flujo, preguntar al usuario antes de tocar.
- Preferir `edit` quirúrgico a `write` masivo.

### 0c. PROBAR TODO, REVISAR TODO, CON CUIDADO
- Probar el fix mínimo ANTES de agregar mejoras.
- Cada cambio: compilar, ejecutar, verificar que funciona.
- Verificar también que NO rompió otra cosa (regresión).
- Si hay duda, preguntar al usuario antes de actuar.
- Revisar logs, procesos, ventanas, salida, todo.

### 0d. USAR LAS MEJORES HERRAMIENTAS
- Usar las herramientas más avanzadas y confiables disponibles.
- Preferir herramientas nativas de Windows (PowerShell, Win32 API) sobre soluciones improvisadas.
- Si una herramienta no funciona en el entorno actual, buscar alternativa o preguntar al usuario.
- No conformarse con una solución mediocre solo porque "funciona en mi máquina".
- Inversión en herramientas de diagnóstico: scripts de debug reutilizables (debug_pinner.ps1).

### 0e. ELEVACIÓN Y SEGURIDAD
- Si se necesita acceso administrador, AVISAR al usuario explícitamente.
- No intentar auto-elevación silenciosa — el usuario debe saber y decidir.
- Crear scripts .ps1 seguros (ExecutionPolicy Bypass solo con -Scope Process).
- Los scripts que el usuario deba ejecutar deben ser:
  - Seguros: no modificar nada sin avisar, no borrar sin confirmar.
  - Autocontenidos: escribir a archivo, no ejecutar comandos destructivos.
  - Revisables: el usuario puede leerlos antes de ejecutarlos.
  - Temporales: limpiar después de usarlos.
- Pedir al usuario ejecutar el script, no ejecutarlo por él si requiere interacción GUI.

## Cómo depurar un launcher que "no funciona"

### 1. ESCUCHAR AL USUARIO
- Preguntar: "¿Qué mensaje de error sale exactamente?"
- Preguntar: "¿Qué esperabas que pasara y qué pasó en su lugar?"
- No asumir nada. No saltar a conclusiones.
- Un proceso que "sigue vivo" NO significa que funciona — puede estar bloqueado por un MessageBox invisible.
- `Start-Process` + `HasExited` NO detecta MessageBox (el proceso espera clic del usuario).

### 2. FLUJO DE DIAGNÓSTICO (orden estricto)

```
1. Escuchar el error exacto que describe el usuario
2. Preguntar detalles si falta claridad
3. Revisar los LOGS primero (AppData, Temp, lado del .exe)
4. Recrear el error desde CLI:
   a. & "ruta\exe"         # ejecuta sincrono, muestra MessageBox
   b. $LASTEXITCODE        # 1 = falló, 0 = CreateProcessW OK
   c. Get-Process -Name powershell  # se creó el proceso hijo?
5. Leer el código ANTES de tocar
   → Buscar donde pinmaker.c resuelve rutas relativas (build_launcher, lines 300-307)
   → Entender el flujo completo desde que el usuario ejecuta hasta CreateProcessW
```

### 2b. HERRAMIENTAS DE DIAGNÓSTICO DISPONIBLES
- **Logs** → Buscar en `exes\*.log`, `%APPDATA%\Pinner\logs\`, `%TEMP%\Pinner\`
- **Procesos** → `Get-Process -Name powershell` antes/después, comparar PIDs
- **Ventanas** → `Add-Type` con `user32.dll` (EnumWindows, GetWindowTextW) para listar títulos
- **Eventos Windows** → `Get-WinEvent -LogName Application` para Application Errors, .NET Runtime
- **Códigos de error** → `$LASTEXITCODE`, `GetLastError()` desde el log del launcher
- **Cadenas en binarios** → `Select-String -Path "exe" -Pattern 'texto'` o `Format-Hex`
- **PATH** → `$env:PATH` para verificar rutas de búsqueda de ejecutables
- **Existencia de archivos** → `Test-Path`, `Get-ChildItem -Filter`
- **Imágenes** → El Read tool soporta leer imágenes (capturas de pantalla, fotos de error)
- **Scripts de debug reutilizables** → Mantener `debug_pinner.ps1` en Temp para tests completos

Siempre intentar primero estas herramientas antes de preguntar al usuario.
Si alguna no está disponible en el entorno, buscar alternativa nativa equivalente.

### 2c. ANÁLISIS DE GUI / UI / UX / SO

**GUI (ventanas y diálogos):**
- Enumerar ventanas visibles con `EnumWindows` + `IsWindowVisible` + `GetWindowTextW`
- Detectar MessageBox por su título y contenido (ventana modal que bloquea)
- Mapear ventanas a procesos con `GetWindowThreadProcessId`
- Identificar diálogos de error (MB_ICONERROR), advertencia (MB_ICONWARNING), información
- Comparar lista de ventanas ANTES y DESPUÉS de ejecutar para detectar nuevas

**UX (experiencia de usuario):**
- Verificar si el launcher se compiló con `--console` (ventana visible) o sin (oculto)
- Detectar si el programa destino es de consola (powershell, cmd, python) y necesita `--console`
- Verificar directorios de trabajo existen antes de lanzar
- Verificar que los argumentos sean correctos (escaping, rutas con espacios)
- Comprobar que el proceso hijo se creó y no murió inmediatamente

**SO (sistema operativo):**
- `Get-WinEvent` para errores de aplicación, .NET Runtime, Application Hang
- Windows version: `[Environment]::OSVersion`, `Get-CimInstance Win32_OperatingSystem`
- Environment variables: `$env:PATH`, `$env:APPDATA`, `$env:TEMP`, `$env:ProgramFiles`
- Permisos: verificar si el proceso necesita admin, si UAC está activo
- Arquitectura: 32 vs 64 bits, rutas System32 vs SysWOW64

### 2d. CÓMO USAR IMÁGENES PARA DIAGNÓSTICO
- No puedo capturar la pantalla del usuario — él debe proporcionar la imagen.
- SI el usuario envía una captura, PUEDO leerla con el Read tool y procesarla.
- Usar Visión IA (modelos multimodales) para:
  - Leer mensajes de error, códigos numéricos, títulos de ventana
  - Identificar botones disponibles y su estado (activo/desactivado)
  - Reconocer íconos (error, advertencia, información, confirmación)
  - Interpretar el contexto visual completo de la pantalla
  - Detectar si hay múltiples diálogos superpuestos
  - Leer texto en imágenes con bajo contraste o fuentes pequeñas
  - Extraer rutas de archivo, directorios, nombres de programa visibles
- Describir la imagen al usuario para confirmar que se entendió el error.
- Si el usuario no sabe tomar captura, indicarle: `Windows+Shift+S` (Recorte y boceto).
- Preguntar siempre: "¿Puedes enviar una captura de pantalla del error?"

### 3. CAUSA RAÍZ MÁS PROBABLE (launcher que no encuentra programa)

**STATUS: CORREGIDO** (commit `8a0f601`)

El bug original: pinmaker.c resolvía rutas relativas a cwd en tiempo de
compilación, inyectando `C:\CurrentDir\powershell.exe` como PROGRAM_PATH.

**Fix aplicado**:
- pinmaker.c ya NO resuelve rutas de programa — las embedde tal cual
- launcher_template.c tiene `ResolvePath(name, ...)` que busca en PATH,
  System32, y WindowsPowerShell en tiempo de ejecución (`src/launcher_template.c:131-167`)
- CreateProcessW recibe appPath vacío cuando el programa no tiene ruta
  absoluta, forzando búsqueda en PATH automática

### 4. NO SOBREINGENIERIZAR
- No agregar funciones nuevas hasta encontrar la causa raíz.
- Primero el fix mínimo que hace funcionar el caso concreto.
- Después las mejoras (logging, validaciones), una por una, probando cada una.
- Si el fix es una línea, NO escribir 200 líneas de código nuevo.

### 5. CÓMO PROBAR CORRECTAMENTE

```powershell
# MAL: no detecta MessageBox (el proceso vive aunque haya error)
$p = Start-Process "launcher.exe" -PassThru; Start-Sleep 2; $p.HasExited

# BIEN: detecta si falló
$p = Start-Process "launcher.exe" -PassThru; Start-Sleep 2
if ($p.HasExited) { "FALLO: $($p.ExitCode)" }
else { $p.Kill(); "OK" }

# MEJOR: verificar que el proceso destino aparece
$before = Get-Process -Name powershell
& "launcher.exe"  # o Start-Process
$after = Get-Process -Name powershell
$after | ? { $_.Id -notin $before.Id }  # nuevo proceso?

# COMPLETO: ventanas antes/después + logs + procesos
# Ver debug_pinner.ps1 como referencia de test completo
```

### 6. PALABRAS CLAVE PARA RECORDAR
- `PROGRAM_PATH` lo inyecta pinmaker — revisar cómo lo arma
- `CreateProcessW(NULL, cmdLine, ...)` busca en PATH automáticamente
- `CreateProcessW(appPath, cmdLine, ...)` usa appPath como ejecutable exacto
- Si appPath no existe → error 2 (file not found) aunque esté en PATH
- Si appPath es relativo a cwd incorrecto → mismo error 2
- Un MessageBox en GUI app se ve como proceso vivo desde CLI
- Si el usuario puede, pedir captura de pantalla o descripción exacta del error visual
- Las imágenes se pueden leer con herramientas de visión si están disponibles
