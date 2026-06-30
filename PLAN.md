# PINNER — Plan de Mejoras

## Filosofía

Facilitar. Personalizar. Agilizar. Humanizar.
La máquina se adapta a vos, no vos a la máquina.

## Mejoras implementadas

### Fase 1 — Launcher autónomo (self-contained) ✅
**Flags:** `--program`, `--args`, `--workdir`
- El .exe lleva el programa y args embebidos (no necesita .ps1 externo)
- Portable: movés el .exe y funciona en cualquier PC
- Soporta: cualquier .exe, script, o comando

### Fase 2 — Modos Admin y Consola ✅
**Flags:** `--admin`, `--console`
- Admin: ejecuta como administrador (ShellExecuteW runas)
- Consola: muestra ventana visible (útil para debug o interacción)

### Fase 3 — Presets integrados ✅
- JSON con configuraciones pre-hechas (Chrome incógnito, Terminal, URL, etc.)
- pinmaker acepta `--preset "Chrome Privado"` como atajo
- GUI los muestra como galería

### Fase 4 — PINNER STUDIO (GUI) ✅
- App visual PowerShell WinForms
- Configura: programa, args, carpeta, icono, nombre, flags
- Vista previa en vivo
- Botón "Generar" → llama a pinmaker.exe
- Galería de presets
- Guardar/Cargar perfiles JSON

### Fase 5 — GUI envuelta como .exe ✅
- pinner-studio.exe generado con pinmaker (autorreferencial)
- Usuario abre y tiene la GUI

### Fase 6 — Generación por lote ✅
- JSON multi-launcher → pinmaker genera todos de una
- Ideal para setup de máquina nueva
- `pinmaker --batch launchers.json`
- Soporta: name, script, program, args, workdir, icon, output, preset, admin, console

---

### Fase 7 — Pinner Python Wizard (GUI Python) ✅
- `pinpywiz.py` — wizard gráfico tkinter para proyectos Python
- Campos: carpeta proyecto, .venv, script Python, nombre, icono, consola
- Genera `.ps1` en `user_scripts\` + ejecuta pinmaker → `.exe` en `exes\`
- Selector de 66+ iconos incluidos + opción de icono personalizado
- Validación de campos antes de generar

### Fase 8 — -NoExit automático en modo consola ✅
- `launcher_template.c`: con `--console` se agrega `-NoExit` a PowerShell
- La ventana se queda abierta al terminar el script (útil para ver errores/output)
- Todos los launchers script se benefician sin modificar cada `.ps1`
- Sin `--console` el comportamiento es el mismo (oculto, sin -NoExit)

---

## Orden de implementación

Cada fase: implementar → probar visualmente → commit

No avanzar hasta que la fase actual funcione perfecto.
