# PINNER — Plan de Mejoras

## Filosofía

Facilitar. Personalizar. Agilizar. Humanizar.
La máquina se adapta a vos, no vos a la máquina.

## Mejoras priorizadas

### Fase 1 — Launcher autónomo (self-contained)
**Flags:** `--program`, `--args`, `--workdir`
- El .exe lleva el programa y args embebidos (no necesita .ps1 externo)
- Portable: movés el .exe y funciona en cualquier PC
- Soporta: cualquier .exe, script, o comando

### Fase 2 — Modos Admin y Consola
**Flags:** `--admin`, `--console`
- Admin: ejecuta como administrador (ShellExecuteW runas)
- Consola: muestra ventana visible (útil para debug o interacción)

### Fase 3 — Presets integrados
- JSON con configuraciones pre-hechas (Chrome incógnito, Terminal, URL, etc.)
- pinmaker acepta `--preset "Chrome Privado"` como atajo
- GUI los muestra como galería

### Fase 4 — PINNER STUDIO (GUI)
- App visual PowerShell WinForms
- Configura: programa, args, carpeta, icono, nombre, flags
- Vista previa en vivo
- Botón "Generar" → llama a pinmaker.exe
- Galería de presets
- Guardar/Cargar perfiles JSON

### Fase 5 — GUI envuelta como .exe
- pinner-studio.exe generado con pinmaker (autorreferencial)
- Usuario abre y tiene la GUI

### Fase 6 — Generación por lote ✅
- JSON multi-launcher → pinmaker genera todos de una
- Ideal para setup de máquina nueva
- `pinmaker --batch launchers.json`
- Soporta: name, script, program, args, workdir, icon, output, preset, admin, console

---

## Orden de implementación

Cada fase: implementar → probar visualmente → commit

No avanzar hasta que la fase actual funcione perfecto.
