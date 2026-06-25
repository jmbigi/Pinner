# PINNER

**Facilitar. Personalizar. Agilizar. Humanizar.**

Pinner genera ejecutables `.exe` de acceso directo a tus programas y scripts
favoritos, con parámetros y configuraciones personalizadas. Sin rodeos,
sin dependencias, sin pedir permiso.

Para que la máquina se adapte a vos, no vos a la máquina.

- **Facilitar** — un comando y tenés tu acceso
- **Personalizar** — icono, nombre, comportamiento, todo a tu gusto
- **Mejorar** — tu flujo de trabajo, no el que Microsoft diseñó para todos
- **Automatizar** — Chrome incógnito, VPN, terminal en tu carpeta, todo a un clic
- **Agilizar** — no navegar menús, no escribir rutas, no repetir pasos
- **Hacer atractivo** — iconos lindos, nombres claros, un taskbar ordenado
- **Volver más humano** — la tecnología sirve al trabajo, el trabajo sirve a la persona

Pinner existe por una convicción simple: **tu escritorio es tuyo, tu barra
de tareas es tuya, y vos decidís cómo querés trabajar.**

---

## ¿Qué es Pinner?

Pinner envuelve cualquier programa o script en un `.exe` PE legítimo de
Windows. El `.exe` generado ejecuta tu programa con los parámetros exactos
que configures, y **se ancla al taskbar como cualquier aplicación**.

Dos modos de uso:

| Modo | Flag | Qué hace |
|------|------|----------|
| **Script** | `--script` | Ejecuta un `.ps1` externo |
| **Programa** | `--program` | Ejecuta cualquier `.exe` con args (autónomo, no necesita script) |

---

## Requisitos

- Windows 10/11 (64-bit)
- [MinGW-w64](https://github.com/brechtsanders/winlibs_mingw/releases) en PATH (`gcc`)
  - Descarga: `winlibs-x86_64-posix-seh-msvcrt.7z`
  - Extraer a `C:\mingw64`, agregar `C:\mingw64\bin` al PATH

## Instalación

```cmd
cd Pinner
build.cmd
```

Esto:
1. Genera `src\default.ico` (icono por defecto "P")
2. Compila `pinmaker.exe` (generador de launchers)
3. Crea carpeta `exes\`
4. Genera `Pinner Studio.exe` (verifica que pinmaker funciona)

---

## PINNER STUDIO (GUI)

La forma más fácil de crear launchers: doble clic y configura visualmente.

```cmd
.\exes\Pinner Studio.exe
```

O desde el código fuente:

```powershell
powershell -ExecutionPolicy Bypass -File src\pinner-studio.ps1
```

La GUI ofrece:
- Formulario completo con todos los campos
- Selector de programa, icono, carpeta de trabajo
- Galería de presets (Chrome, Terminal, VS Code, etc.)
- Vista previa en vivo
- Botón "Generar" que llama a `pinmaker.exe`
- Abre la carpeta `exes\` al finalizar

---

## Uso CLI

### Generar un launcher (modo programa — autónomo)

```cmd
pinmaker -n "Chrome Privado" -p "chrome.exe" -a "-incognito" -i chrome.ico
```

El `.exe` generado **no necesita ningún archivo externo** — el programa y
argumentos van embebidos.

### Generar un launcher (modo script)

```cmd
pinmaker -n "Mi Script" -s "C:\ruta\script.ps1"
```

El `.exe` busca el script en la ruta absoluta al ejecutarse.

### Anclar al taskbar

```cmd
cd exes
# Clic derecho en "Chrome Privado.exe" -> "Anclar a la barra de tareas"
```

---

## Opciones completas

| Flag | Descripción |
|------|-------------|
| `--name`, `-n` | Nombre del acceso (obligatorio) |
| `--script`, `-s` | Ruta al script `.ps1` (alternativa a `--program`) |
| `--program`, `-p` | Ruta al programa `.exe` (alternativa a `--script`) |
| `--args`, `-a` | Argumentos del programa |
| `--workdir`, `-d` | Carpeta de trabajo |
| `--icon`, `-i` | Ruta al archivo `.ico` |
| `--preset` | Nombre del preset (ver `--preset list`) |
| `--admin` | Ejecutar como administrador |
| `--console` | Mostrar ventana de consola |
| `--output`, `-o` | Carpeta de salida (default: `.\exes\`) |
| `--batch` | Archivo JSON con múltiples launchers para generar en lote |
| `--help`, `-h` | Muestra ayuda |

---

## Presets

Usa plantillas pre-configuradas para los casos más comunes:

```cmd
pinmaker --preset list    # Lista todos los presets
pinmaker -n "Mi Chrome" --preset "Chrome Privado"
pinmaker -n "Terminal" --preset "Terminal en carpeta" -d "C:\Proyecto"
```

Presets disponibles: Chrome Privado, Chrome Perfil Trabajo, Edge Privado,
Firefox Privado, Terminal en carpeta, Terminal Admin, Símbolo del sistema,
Abrir URL, Bloc de notas, Explorador, VS Code, Python Script.

Los flags explícitos sobreescriben los valores del preset. Ej:

```cmd
pinmaker -n "Terminal" --preset "Terminal en carpeta" -d "C:\MiProyecto"
```

---

## Ejemplos

### Chrome modo incógnito con icono

```cmd
pinmaker -n "Chrome Privado" -p "chrome.exe" -a "-incognito" -i icons\ico\chrome.ico
```

### Terminal en carpeta de trabajo

```cmd
pinmaker -n "PS Proyecto" -p "powershell.exe" -a "-NoExit -Command Set-Location 'C:\Proyecto'" -i icons\ico\powershell.ico
```

### Ejecutar como administrador

```cmd
pinmaker -n "VPN Empresa" -p "C:\VPN\client.exe" --admin
```

### App Python con consola visible

```cmd
pinmaker -n "Mi App" -p "python.exe" -a "app.py" -d "C:\Proyecto" --console -i icons\ico\python.ico
```

### Abrir URL específica

```cmd
pinmaker -n "Portal" -p "cmd.exe" -a "/c start https://portal.cia"
```

### Script personalizado

```cmd
pinmaker -n "Backup" -s "C:\scripts\backup.ps1" --admin --console
```

### Batch desde JSON (múltiples launchers de una sola vez)

```cmd
pinmaker --batch launchers.json
```

Ejemplo de `launchers.json`:
```json
[
  {
    "name": "Chrome Privado",
    "program": "chrome.exe",
    "args": "-incognito",
    "icon": "icons/ico/chrome.ico"
  },
  {
    "name": "Terminal Admin",
    "program": "powershell.exe",
    "args": "-NoExit",
    "admin": true,
    "console": true,
    "icon": "icons/ico/powershell.ico"
  },
  {
    "name": "Backup Diario",
    "script": "C:\\scripts\\backup.ps1",
    "admin": true,
    "console": true
  }
]
```

Soporta por cada item: `name`, `program`, `script`, `args`, `workdir`, `icon`, `preset`, `admin`, `console`, `output`.

---

## Cómo funciona

```
pinmaker [args]
  → lee src/launcher_template.c
  → inyecta defines (APP_NAME, PROGRAM_PATH, etc.)
  → si hay icono: .rc + windres -O coff
  → gcc -mwindows -Os -s (GUI, mínimo peso)
  → .exe listo para anclar
```

El `.exe` generado:
- Es un ejecutable PE Windows estándar (anclable al taskbar)
- Pesa ~20-100 KB (dependiendo del icono)
- No requiere dependencias externas
- No necesita administrador (a menos que se use `--admin`)
- Se puede renombrar, mover, copiar libremente
- Soporta paso de argumentos desde el acceso directo

---

## PINNER STUDIO (GUI)

La interfaz gráfica permite crear launchers sin usar la terminal.

Se abre como cualquier programa:
```cmd
.\exes\Pinner Studio.exe
```

Características:
- **Formulario completo**: nombre, programa, argumentos, carpeta, icono
- **Selectores de archivos**: botones "..." para elegir programa, carpeta, icono
- **Presets**: dropdown con configuraciones pre-hechas
- **Vista previa**: muestra el comando exacto antes de generar
- **Flags visuales**: checkboxes para admin y consola
- **Resultado**: mensaje claro y opción de abrir la carpeta `exes\`

La GUI está escrita en PowerShell WinForms — cero dependencias externas.
Se ejecuta en cualquier Windows con PowerShell 5+ (viene instalado).

---

## Estructura del proyecto

```
Pinner\
├── src\
│   ├── pinmaker.c              # Generador de launchers (C)
│   ├── launcher_template.c     # Template C del launcher
│   ├── presets.h               # Presets integrados
│   ├── pinner-studio.ps1       # GUI (PowerShell WinForms)
│   └── default.ico             # Icono por defecto
├── exes\                       # Launchers generados (gitignored)
│   └── Pinner Studio.exe       # GUI envuelta como .exe
├── icons\                      # Colección de iconos (SVG + ICO)
│   ├── svg\                    #   Fuentes SVG
│   └── ico\                    #   ~60 iconos .ico listos para usar
├── build.cmd                   # Compila pinmaker.exe
├── gen_icon.ps1                # Genera src\default.ico
├── convert_svg_to_ico.ps1      # Convierte SVGs a ICOs
├── download_icons.ps1          # Descarga iconos desde SimpleIcons
├── pinmaker.exe                # Generador (ya compilado, gitignored)
├── PLAN.md                     # Hoja de ruta
├── LICENSE                     # Licencia MIT
├── .gitignore
└── README.md
```

---

## Filosofía

La personalización del entorno de trabajo no es un lujo — es agilidad,
productividad, y humanidad. Cada persona trabaja diferente, piensa diferente,
usa herramientas diferentes. Un entorno que no se adapta a quien lo usa
es un entorno que frena, no que impulsa.

Pinner devuelve el control a quien usa la máquina. Sin pedir permiso,
sin pasar por una tienda, sin depender de políticas corporativas.
Software libre, compilación local, cero dependencias externas.

> La tecnología está para servir al trabajo.
> El trabajo está para servir a la persona.
> Pinner está para que no te olvides de eso.

---

## Licencia

MIT — Haz lo que quieras. Ver [LICENSE](LICENSE).
