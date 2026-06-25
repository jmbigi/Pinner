#Requires -Version 5
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

function Find-PinnerRoot {
    $dir = $ScriptDir
    for ($i = 0; $i -lt 3; $i++) {
        if (Test-Path (Join-Path $dir "pinmaker.exe")) { return $dir }
        $dir = Split-Path -Parent $dir
        if (-not $dir) { break }
    }
    return $ScriptDir
}

$PinnerRoot = Find-PinnerRoot
$Pinmaker = Join-Path $PinnerRoot "pinmaker.exe"
$IconDir = Join-Path $PinnerRoot "icons\ico"
$ExeDir = Join-Path $PinnerRoot "exes"

if (-not (Test-Path $Pinmaker)) {
    [System.Windows.Forms.MessageBox]::Show(
        "No se encuentra pinmaker.exe`nBuscado en: $Pinmaker",
        "PINNER STUDIO", [System.Windows.Forms.MessageBoxButtons]::OK,
        [System.Windows.Forms.MessageBoxIcon]::Error)
    exit 1
}

# --- Presets ---
$presets = @(
    @{Name="(Personalizado)"; Program=""; Args=""; WorkDir=""; Admin=$false; Console=$false; Icon=""},
    @{Name="Chrome Privado"; Program="chrome.exe"; Args="-incognito"; WorkDir=""; Admin=$false; Console=$false; Icon="chrome.ico"},
    @{Name="Chrome Perfil Trabajo"; Program="chrome.exe"; Args='--profile-directory="Profile 1"'; WorkDir=""; Admin=$false; Console=$false; Icon="chrome.ico"},
    @{Name="Edge Privado"; Program="msedge.exe"; Args="-inprivate"; WorkDir=""; Admin=$false; Console=$false; Icon="microsoft-edge.ico"},
    @{Name="Firefox Privado"; Program="firefox.exe"; Args="-private-window"; WorkDir=""; Admin=$false; Console=$false; Icon="firefox.ico"},
    @{Name="Terminal en carpeta"; Program="powershell.exe"; Args="-NoExit -Command Set-Location '.'"; WorkDir=""; Admin=$false; Console=$true; Icon="powershell.ico"},
    @{Name="Terminal Admin"; Program="powershell.exe"; Args="-NoExit"; WorkDir=""; Admin=$true; Console=$true; Icon="powershell.ico"},
    @{Name="Simbolo del sistema"; Program="cmd.exe"; Args="/k"; WorkDir=""; Admin=$false; Console=$true; Icon="terminal.ico"},
    @{Name="Abrir URL"; Program="cmd.exe"; Args='/c start'; WorkDir=""; Admin=$false; Console=$false; Icon="chrome.ico"},
    @{Name="Bloc de notas"; Program="notepad.exe"; Args=""; WorkDir=""; Admin=$false; Console=$false; Icon="notepad.ico"},
    @{Name="Explorador"; Program="explorer.exe"; Args=""; WorkDir=""; Admin=$false; Console=$false; Icon="windows.ico"},
    @{Name="VS Code"; Program="code.exe"; Args="."; WorkDir=""; Admin=$false; Console=$true; Icon="visual-studio-code.ico"},
    @{Name="Python Script"; Program="python.exe"; Args=""; WorkDir=""; Admin=$false; Console=$true; Icon="python.ico"},
    @{Name="PS desviaciones"; Program="powershell.exe"; Args="-NoExit -Command Set-Location 'C:\Collahuasi\GitLab\desviaciones'"; WorkDir=""; Admin=$false; Console=$false; Icon="powershell.ico"}
)

# --- Build form ---
$form = New-Object System.Windows.Forms.Form
$form.Text = "PINNER STUDIO — Genera tu launcher"
$form.Size = New-Object System.Drawing.Size(620, 560)
$form.StartPosition = "CenterScreen"
$form.Font = New-Object System.Drawing.Font("Segoe UI", 10)
$form.Icon = [System.Drawing.Icon]::ExtractAssociatedIcon($Pinmaker)

# --- Controls ---
$y = 20
$labelW = 120
$fieldW = 340
$btnW = 30

# Preset
New-Object System.Windows.Forms.Label -Property @{
    Text = "Preset:"; Location = New-Object System.Drawing.Point(20, $y + 4)
    Size = New-Object System.Drawing.Size($labelW, 20)
    Parent = $form
}
$cmbPreset = New-Object System.Windows.Forms.ComboBox -Property @{
    Location = New-Object System.Drawing.Point(140, $y)
    Size = New-Object System.Drawing.Size($fieldW, 24)
    DropDownStyle = "DropDownList"; Parent = $form
}
$cmbPreset.Items.AddRange([string[]]($presets | ForEach-Object { $_.Name }))
$cmbPreset.SelectedIndex = 0

$y += 35

# Name
New-Object System.Windows.Forms.Label -Property @{
    Text = "Nombre:"; Location = New-Object System.Drawing.Point(20, $y + 4)
    Size = New-Object System.Drawing.Size($labelW, 20); Parent = $form
}
$txtName = New-Object System.Windows.Forms.TextBox -Property @{
    Location = New-Object System.Drawing.Point(140, $y)
    Size = New-Object System.Drawing.Size($fieldW, 24); Parent = $form
}

$y += 35

# Program
New-Object System.Windows.Forms.Label -Property @{
    Text = "Programa:"; Location = New-Object System.Drawing.Point(20, $y + 4)
    Size = New-Object System.Drawing.Size($labelW, 20); Parent = $form
}
$txtProgram = New-Object System.Windows.Forms.TextBox -Property @{
    Location = New-Object System.Drawing.Point(140, $y)
    Size = New-Object System.Drawing.Size($fieldW - $btnW - 5, 24); Parent = $form
}
$btnProgram = New-Object System.Windows.Forms.Button -Property @{
    Text = "..."; Location = New-Object System.Drawing.Point(140 + $fieldW - $btnW - 5, $y - 1)
    Size = New-Object System.Drawing.Size($btnW + 5, 26); Parent = $form
}

$y += 35

# Args
New-Object System.Windows.Forms.Label -Property @{
    Text = "Argumentos:"; Location = New-Object System.Drawing.Point(20, $y + 4)
    Size = New-Object System.Drawing.Size($labelW, 20); Parent = $form
}
$txtArgs = New-Object System.Windows.Forms.TextBox -Property @{
    Location = New-Object System.Drawing.Point(140, $y)
    Size = New-Object System.Drawing.Size($fieldW, 24); Parent = $form
}

$y += 35

# WorkDir
New-Object System.Windows.Forms.Label -Property @{
    Text = "Carpeta:"; Location = New-Object System.Drawing.Point(20, $y + 4)
    Size = New-Object System.Drawing.Size($labelW, 20); Parent = $form
}
$txtWorkDir = New-Object System.Windows.Forms.TextBox -Property @{
    Location = New-Object System.Drawing.Point(140, $y)
    Size = New-Object System.Drawing.Size($fieldW - $btnW - 5, 24); Parent = $form
}
$btnWorkDir = New-Object System.Windows.Forms.Button -Property @{
    Text = "..."; Location = New-Object System.Drawing.Point(140 + $fieldW - $btnW - 5, $y - 1)
    Size = New-Object System.Drawing.Size($btnW + 5, 26); Parent = $form
}

$y += 35

# Icon
New-Object System.Windows.Forms.Label -Property @{
    Text = "Icono:"; Location = New-Object System.Drawing.Point(20, $y + 4)
    Size = New-Object System.Drawing.Size($labelW, 20); Parent = $form
}
$txtIcon = New-Object System.Windows.Forms.TextBox -Property @{
    Location = New-Object System.Drawing.Point(140, $y)
    Size = New-Object System.Drawing.Size($fieldW - $btnW - 5, 24); Parent = $form
}
$btnIcon = New-Object System.Windows.Forms.Button -Property @{
    Text = "..."; Location = New-Object System.Drawing.Point(140 + $fieldW - $btnW - 5, $y - 1)
    Size = New-Object System.Drawing.Size($btnW + 5, 26); Parent = $form
}
$picIcon = New-Object System.Windows.Forms.PictureBox -Property @{
    Location = New-Object System.Drawing.Point(140 + $fieldW + 10, $y - 2)
    Size = New-Object System.Drawing.Size(32, 32)
    SizeMode = "StretchImage"; Parent = $form
    BorderStyle = "FixedSingle"
}

$y += 45

# Flags
$chkAdmin = New-Object System.Windows.Forms.CheckBox -Property @{
    Text = "Ejecutar como administrador"; Location = New-Object System.Drawing.Point(140, $y)
    Size = New-Object System.Drawing.Size(220, 24); Parent = $form
}
$chkConsole = New-Object System.Windows.Forms.CheckBox -Property @{
    Text = "Mostrar ventana"; Location = New-Object System.Drawing.Point(370, $y)
    Size = New-Object System.Drawing.Size(140, 24); Parent = $form
}

$y += 35

# Preview box
$grpPreview = New-Object System.Windows.Forms.GroupBox -Property @{
    Text = "Vista previa"; Location = New-Object System.Drawing.Point(20, $y)
    Size = New-Object System.Drawing.Size(560, 80); Parent = $form
}
$lblPreview = New-Object System.Windows.Forms.Label -Property @{
    Location = New-Object System.Drawing.Point(10, 20)
    Size = New-Object System.Drawing.Size(540, 50); Parent = $grpPreview
    ForeColor = "DarkGreen"
}

$y += 100

# Generate button
$btnGenerate = New-Object System.Windows.Forms.Button -Property @{
    Text = "GENERAR .EXE"; Location = New-Object System.Drawing.Point(140, $y)
    Size = New-Object System.Drawing.Size(200, 40); Parent = $form
    BackColor = "DodgerBlue"; ForeColor = "White"
    FlatStyle = "Flat"; Font = New-Object System.Drawing.Font("Segoe UI", 12, [System.Drawing.FontStyle]::Bold)
}
$btnGenerate.FlatAppearance.BorderSize = 0

$btnCancel = New-Object System.Windows.Forms.Button -Property @{
    Text = "Cancelar"; Location = New-Object System.Drawing.Point(360, $y)
    Size = New-Object System.Drawing.Size(120, 40); Parent = $form
}

$y += 55

# Status label
$lblStatus = New-Object System.Windows.Forms.Label -Property @{
    Location = New-Object System.Drawing.Point(20, $y)
    Size = New-Object System.Drawing.Size(560, 50); Parent = $form
    ForeColor = "DarkBlue"
}

# --- Events ---
$cmbPreset.Add_SelectedIndexChanged({
    $p = $presets[$cmbPreset.SelectedIndex]
    if ($p.Name -eq "(Personalizado)") { return }
    $txtProgram.Text = $p.Program
    $txtArgs.Text = $p.Args
    $txtWorkDir.Text = $p.WorkDir
    $chkAdmin.Checked = $p.Admin
    $chkConsole.Checked = $p.Console
    if ($p.Icon -and (Test-Path (Join-Path $IconDir $p.Icon))) {
        $txtIcon.Text = Join-Path $IconDir $p.Icon
        Update-IconPreview
    }
    Update-Preview
})

$txtName.Add_TextChanged({ Update-Preview })
$txtProgram.Add_TextChanged({ Update-Preview })
$txtArgs.Add_TextChanged({ Update-Preview })
$txtWorkDir.Add_TextChanged({ Update-Preview })
$chkAdmin.Add_CheckedChanged({ Update-Preview })
$chkConsole.Add_CheckedChanged({ Update-Preview })

function Update-Preview {
    $name = $txtName.Text
    $prog = $txtProgram.Text
    $args = $txtArgs.Text
    $dir = $txtWorkDir.Text
    $admin = $chkAdmin.Checked
    $console = $chkConsole.Checked

    if (-not $name) { $lblPreview.Text = "Completa los campos para ver la vista previa"; return }

    $lines = @()
    $lines += "$name.exe"
    $lines += "→ $prog $args"
    if ($dir) { $lines += "  Carpeta: $dir" }
    $flags = @()
    if ($admin) { $flags += "ADMIN" }
    if ($console) { $flags += "CONSOLA" } else { $flags += "SILENCIOSO" }
    $lines += "  [$($flags -join ' + ')]"
    $lblPreview.Text = $lines -join "`n"
}

function Update-IconPreview {
    if ($txtIcon.Text -and (Test-Path $txtIcon.Text)) {
        try { $picIcon.Image = [System.Drawing.Icon]::ExtractAssociatedIcon($txtIcon.Text) }
        catch { $picIcon.Image = $null }
    } else { $picIcon.Image = $null }
}

$btnProgram.Add_Click({
    $dlg = New-Object System.Windows.Forms.OpenFileDialog
    $dlg.Filter = "Ejecutables (*.exe;*.com;*.cmd;*.bat)|*.exe;*.com;*.cmd;*.bat|Todos (*.*)|*.*"
    $dlg.Title = "Seleccionar programa"
    if ($dlg.ShowDialog() -eq "OK") { $txtProgram.Text = $dlg.FileName; Update-Preview }
})

$btnWorkDir.Add_Click({
    $dlg = New-Object System.Windows.Forms.FolderBrowserDialog
    $dlg.Description = "Seleccionar carpeta de trabajo"
    if ($txtWorkDir.Text -and (Test-Path $txtWorkDir.Text)) { $dlg.SelectedPath = $txtWorkDir.Text }
    if ($dlg.ShowDialog() -eq "OK") { $txtWorkDir.Text = $dlg.SelectedPath; Update-Preview }
})

$btnIcon.Add_Click({
    $dlg = New-Object System.Windows.Forms.OpenFileDialog
    $dlg.Filter = "Iconos (*.ico;*.exe;*.dll)|*.ico;*.exe;*.dll|Todos (*.*)|*.*"
    $dlg.Title = "Seleccionar icono"
    if ($dlg.ShowDialog() -eq "OK") { $txtIcon.Text = $dlg.FileName; Update-IconPreview }
})
$txtIcon.Add_TextChanged({ Update-IconPreview })

$btnGenerate.Add_Click({
    $name = $txtName.Text.Trim()
    if (-not $name) { $lblStatus.Text = "ERROR: Escribe un nombre"; return }
    $prog = $txtProgram.Text.Trim()
    if (-not $prog) { $lblStatus.Text = "ERROR: Escribe o selecciona un programa"; return }
    $args = $txtArgs.Text.Trim()
    $dir = $txtWorkDir.Text.Trim()
    $icon = $txtIcon.Text.Trim()
    $admin = $chkAdmin.Checked
    $console = $chkConsole.Checked

    $lblStatus.Text = "Generando..."
    $lblStatus.ForeColor = "DarkBlue"
    $btnGenerate.Enabled = $false
    [System.Windows.Forms.Application]::DoEvents()

    # Build pinmaker args
    $pmArgs = @()
    $pmArgs += "-n"; $pmArgs += $name
    $pmArgs += "-p"; $pmArgs += $prog
    if ($args) { $pmArgs += "-a"; $pmArgs += $args }
    if ($dir) { $pmArgs += "-d"; $pmArgs += $dir }
    if ($admin) { $pmArgs += "--admin" }
    if ($console) { $pmArgs += "--console" }
    if ($icon -and (Test-Path $icon)) { $pmArgs += "-i"; $pmArgs += $icon }
    $pmArgs += "-o"; $pmArgs += $ExeDir

    $proc = Start-Process -FilePath $Pinmaker -ArgumentList $pmArgs -NoNewWindow -Wait -PassThru -RedirectStandardOutput "$env:TEMP\pinner_out.txt" -RedirectStandardError "$env:TEMP\pinner_err.txt"
    $output = Get-Content "$env:TEMP\pinner_out.txt" -Raw
    $err = Get-Content "$env:TEMP\pinner_err.txt" -Raw

    $btnGenerate.Enabled = $true

    if ($proc.ExitCode -eq 0) {
        $lblStatus.Text = "OK: $name.exe generado en exes\"
        $lblStatus.ForeColor = "DarkGreen"
        $result = [System.Windows.Forms.MessageBox]::Show(
            "$name.exe generado correctamente.`n`nAbrir carpeta exes y anclar al taskbar?",
            "PINNER STUDIO", [System.Windows.Forms.MessageBoxButtons]::YesNo,
            [System.Windows.Forms.MessageBoxIcon]::Question)
        if ($result -eq "Yes") {
            Start-Process (Join-Path $ScriptDir "exes")
        }
    } else {
        $lblStatus.Text = "ERROR: Compilacion fallo. Revisa que MinGW-w64 este en PATH"
        $lblStatus.ForeColor = "DarkRed"
        [System.Windows.Forms.MessageBox]::Show(
            "Error al generar el launcher.`n`n$output`n$err",
            "Error", [System.Windows.Forms.MessageBoxButtons]::OK,
            [System.Windows.Forms.MessageBoxIcon]::Error)
    }
})

$btnCancel.Add_Click({ $form.Close() })

# --- Show ---
$form.ShowDialog() | Out-Null
