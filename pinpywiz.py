"""Pinner Python Wizard — GUI para crear launchers de proyectos Python."""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import os
import re
import subprocess
import sys
from pathlib import Path


def valid_filename(name):
    invalid = '<>:"/\\|?*'
    return not any(c in name for c in invalid)


def make_relative(path, base):
    try:
        return os.path.relpath(path, base).replace("\\", "/")
    except ValueError:
        return path.replace("\\", "/")


def load_icons(icons_dir):
    items = [("Default (P)", "default")]
    if icons_dir and icons_dir.exists():
        for f in sorted(icons_dir.iterdir()):
            if f.suffix.lower() == ".ico":
                items.append((f.stem, str(f)))
    items.append(("Otro (examinar...)", "__browse__"))
    return items


def generate_script(project, venv, script, name):
    script_rel = make_relative(script, project)
    lines = []
    lines.append(f'Write-Host "=== {name} ===" -ForegroundColor Cyan')
    lines.append(f'Write-Host "Proyecto: {project}" -ForegroundColor Gray')
    lines.append("")
    lines.append(f'$projectRoot = "{project}"')
    lines.append('if (-not (Test-Path $projectRoot)) {')
    lines.append('    Write-Host "ERROR: No existe $projectRoot" -ForegroundColor Red')
    lines.append('    return')
    lines.append('}')
    lines.append("")
    lines.append("Set-Location $projectRoot")
    if venv:
        activate_path = _get_activate_path(venv)
        lines.append("")
        lines.append('Write-Host "Activando entorno virtual..." -ForegroundColor Yellow')
        lines.append('try {')
        lines.append(f'    . "{activate_path}"')
        lines.append('} catch {')
        lines.append('    Write-Host "ERROR al activar entorno: $_" -ForegroundColor Red')
        lines.append('    return')
        lines.append('}')
    lines.append("")
    lines.append(f'Write-Host "Ejecutando: python {script_rel}" -ForegroundColor Green')
    lines.append(f'python "{script_rel}"')
    lines.append("")
    lines.append('if ($LASTEXITCODE -ne 0) {')
    lines.append('    Write-Host "`nERROR: Python terminó con código $LASTEXITCODE" -ForegroundColor Red')
    lines.append('} else {')
    lines.append('    Write-Host "`nOK - Finalizado correctamente" -ForegroundColor Green')
    lines.append('}')
    return "\n".join(lines) + "\n"


def _get_activate_path(venv):
    p = Path(venv)
    if p.suffix.lower() == ".ps1":
        return str(p)
    return str(p / "Scripts" / "Activate.ps1")


def save_script(scripts_dir, name, content):
    safe = re.sub(r'[^\w\- ]', '', name).strip() or "script"
    filename = safe.replace(" ", "_").lower() + ".ps1"
    path = Path(scripts_dir) / filename
    Path(scripts_dir).mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8")
    return path


def run_pinmaker(pinmaker_exe, name, script_path, exes_dir, icon_val, use_console):
    args = [
        str(pinmaker_exe),
        "-n", name,
        "-s", str(script_path),
        "-o", str(exes_dir),
    ]
    if use_console:
        args.append("--console")
    if icon_val and icon_val != "default":
        args.extend(["-i", icon_val])

    Path(exes_dir).mkdir(parents=True, exist_ok=True)
    result = subprocess.run(args, capture_output=True, text=True,
                            cwd=str(Path(pinmaker_exe).parent))
    return result


class PinnerPyWizard:
    def __init__(self):
        self.pinner_dir = Path(__file__).resolve().parent
        self.icons_dir = self.pinner_dir / "icons" / "ico"
        self.user_scripts_dir = self.pinner_dir / "user_scripts"
        self.exes_dir = self.pinner_dir / "exes"
        self.pinmaker_exe = self.pinner_dir / "pinmaker.exe"

        self.var_project = tk.StringVar()
        self.var_venv = tk.StringVar()
        self.var_script = tk.StringVar()
        self.var_name = tk.StringVar()
        self.var_icon = tk.StringVar(value="default")
        self.var_console = tk.BooleanVar(value=True)

        self.icon_list = load_icons(self.icons_dir)

        self.root = tk.Tk()
        self.root.title("Pinner Python Wizard")
        self.root.resizable(False, False)
        self._build_ui()
        self.root.mainloop()

    def _browse_folder(self, var, title="Seleccionar carpeta", initial=None):
        initial_dir = initial or var.get() or str(self.pinner_dir)
        folder = filedialog.askdirectory(title=title, initialdir=initial_dir)
        if folder:
            var.set(folder)

    def _browse_file(self, var, title="Seleccionar archivo", initial=None, filetypes=None):
        initial_dir = initial or (os.path.dirname(var.get()) if var.get() else str(self.pinner_dir))
        filetypes = filetypes or [("Python scripts", "*.py"), ("Todos", "*.*")]
        filepath = filedialog.askopenfilename(title=title, initialdir=initial_dir, filetypes=filetypes)
        if filepath:
            var.set(filepath)

    def _browse_script(self):
        initial = self.var_project.get() or str(self.pinner_dir)
        self._browse_file(self.var_script, "Seleccionar script Python", initial)

    def _browse_icon_file(self):
        filepath = filedialog.askopenfilename(
            title="Seleccionar ícono",
            initialdir=str(self.icons_dir),
            filetypes=[("Iconos", "*.ico"), ("Todos", "*.*")]
        )
        if filepath:
            self.var_icon.set(filepath)

    def _on_icon_select(self, event=None):
        selected = self.icon_combo.get()
        for _, value in self.icon_list:
            if selected == _:
                if value == "__browse__":
                    self._browse_icon_file()
                else:
                    self.var_icon.set(value)
                break

    def _validate(self):
        errors = []
        project = self.var_project.get().strip()
        if not project:
            errors.append("Carpeta del proyecto es requerida")
        elif not Path(project).exists():
            errors.append(f"La carpeta del proyecto no existe:\n{project}")

        venv = self.var_venv.get().strip()
        if venv and not Path(venv).exists():
            errors.append(f"La carpeta .venv no existe:\n{venv}")

        script = self.var_script.get().strip()
        if not script:
            errors.append("Script Python es requerido")
        elif not Path(script).exists():
            errors.append(f"El script no existe:\n{script}")

        name = self.var_name.get().strip()
        if not name:
            errors.append("Nombre del acceso es requerido")
        elif not valid_filename(name):
            errors.append("El nombre contiene caracteres no válidos\n(< > : \" / \\ | ? *)")

        if not self.pinmaker_exe.exists():
            errors.append(f"No se encuentra pinmaker.exe.\nEjecutá build.cmd primero.")

        return errors

    def _generate(self):
        errors = self._validate()
        if errors:
            messagebox.showerror("Errores de validación", "\n\n".join(errors))
            return

        project = self.var_project.get().strip()
        venv = self.var_venv.get().strip()
        script = self.var_script.get().strip()
        name = self.var_name.get().strip()
        icon_val = self.var_icon.get()
        use_console = self.var_console.get()

        ps1_content = generate_script(project, venv, script, name)

        try:
            ps1_path = save_script(self.user_scripts_dir, name, ps1_content)
        except Exception as e:
            messagebox.showerror("Error al guardar script", str(e))
            return

        try:
            result = run_pinmaker(self.pinmaker_exe, name, ps1_path,
                                  self.exes_dir, icon_val, use_console)
            output = result.stdout + result.stderr
            if result.returncode == 0:
                exe_path = self.exes_dir / f"{name}.exe"
                msg = (
                    f"Launcher creado exitosamente:\n\n"
                    f"  {exe_path}\n\n"
                    f"Script generado:\n"
                    f"  {ps1_path}\n\n"
                    f"Ahora anclalo al taskbar:\n"
                    f"  1. Clic derecho -> Anclar a la barra de tareas\n"
                    f"  2. Arrastrar a la posición deseada"
                )
                messagebox.showinfo("Éxito", msg)
            else:
                messagebox.showerror("Error al compilar", output)
        except Exception as e:
            messagebox.showerror("Error al ejecutar pinmaker", str(e))

    def _build_ui(self):
        main = ttk.Frame(self.root, padding=16)
        main.grid(row=0, column=0, sticky="nsew")

        title = ttk.Label(main, text="Pinner Python Wizard",
                          font=("Segoe UI", 14, "bold"))
        title.grid(row=0, column=0, columnspan=3, pady=(0, 16))

        row = 1
        # Project folder
        ttk.Label(main, text="Carpeta del proyecto:").grid(
            row=row, column=0, sticky="w", pady=2)
        ttk.Entry(main, textvariable=self.var_project, width=50).grid(
            row=row, column=1, padx=4, pady=2)
        ttk.Button(main, text="Examinar",
                   command=lambda: self._browse_folder(
                       self.var_project, "Carpeta del proyecto")
                   ).grid(row=row, column=2, pady=2)
        row += 1

        # .venv folder
        ttk.Label(main, text="Carpeta .venv:").grid(
            row=row, column=0, sticky="w", pady=2)
        ttk.Entry(main, textvariable=self.var_venv, width=50).grid(
            row=row, column=1, padx=4, pady=2)
        ttk.Button(main, text="Examinar",
                   command=lambda: self._browse_folder(
                       self.var_venv, "Carpeta del entorno virtual",
                       initial=self.var_project.get() or str(self.pinner_dir))
                   ).grid(row=row, column=2, pady=2)
        row += 1

        # Script path
        ttk.Label(main, text="Script Python:").grid(
            row=row, column=0, sticky="w", pady=2)
        ttk.Entry(main, textvariable=self.var_script, width=50).grid(
            row=row, column=1, padx=4, pady=2)
        ttk.Button(main, text="Examinar",
                   command=self._browse_script).grid(
            row=row, column=2, pady=2)
        row += 1

        # Separator
        ttk.Separator(main, orient="horizontal").grid(
            row=row, column=0, columnspan=3, sticky="ew", pady=8)
        row += 1

        # Launcher name
        ttk.Label(main, text="Nombre del acceso:").grid(
            row=row, column=0, sticky="w", pady=2)
        ttk.Entry(main, textvariable=self.var_name, width=50).grid(
            row=row, column=1, columnspan=2, sticky="w", padx=4, pady=2)
        row += 1

        # Icon selector
        ttk.Label(main, text="Ícono:").grid(
            row=row, column=0, sticky="w", pady=2)
        display_names = [item[0] for item in self.icon_list]
        self.icon_combo = ttk.Combobox(
            main, values=display_names, width=47, state="readonly")
        self.icon_combo.set(display_names[0])
        self.icon_combo.grid(
            row=row, column=1, columnspan=2, sticky="w", padx=4, pady=2)
        self.icon_combo.bind("<<ComboboxSelected>>", self._on_icon_select)
        row += 1

        # Console checkbox
        ttk.Checkbutton(
            main, text="Mostrar ventana de consola (--console)",
            variable=self.var_console
        ).grid(row=row, column=0, columnspan=3, sticky="w", pady=4)
        row += 1

        # Separator
        ttk.Separator(main, orient="horizontal").grid(
            row=row, column=0, columnspan=3, sticky="ew", pady=8)
        row += 1

        # Buttons
        btn_frame = ttk.Frame(main)
        btn_frame.grid(row=row, column=0, columnspan=3, pady=8)
        ttk.Button(btn_frame, text="Generar y compilar",
                   command=self._generate, width=20).pack(side="left", padx=4)
        ttk.Button(btn_frame, text="Cancelar",
                   command=self.root.destroy, width=12).pack(side="left", padx=4)

        # Keyboard shortcuts
        self.root.bind("<Return>", lambda e: self._generate())


if __name__ == "__main__":
    PinnerPyWizard()
