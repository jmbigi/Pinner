param(
    [string]$OutputDir = ".\icons\svg"
)

$OutputDir = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutputDir)
$ErrorActionPreference = "Stop"

$apps = @(
    @{Slug="googlechrome";          File="chrome.svg"}
    @{Slug="firefoxbrowser";        File="firefox.svg"}
    @{Slug="microsoftedge";         File="microsoft-edge.svg"}
    @{Slug="brave";                 File="brave.svg"}
    @{Slug="opera";                 File="opera.svg"}
    @{Slug="vivaldi";               File="vivaldi.svg"}
    @{Slug="visualstudiocode";      File="visual-studio-code.svg"}
    @{Slug="sublimetext";           File="sublime-text.svg"}
    @{Slug="notepadplusplus";       File="notepad-plus-plus.svg"}
    @{Slug="neovim";                File="neovim.svg"}
    @{Slug="intellijidea";          File="intellij-idea.svg"}
    @{Slug="androidstudio";         File="android-studio.svg"}
    @{Slug="pycharm";               File="pycharm.svg"}
    @{Slug="spotify";               File="spotify.svg"}
    @{Slug="discord";               File="discord.svg"}
    @{Slug="telegram";              File="telegram.svg"}
    @{Slug="whatsapp";              File="whatsapp.svg"}
    @{Slug="slack";                 File="slack.svg"}
    @{Slug="zoom";                  File="zoom.svg"}
    @{Slug="steam";                 File="steam.svg"}
    @{Slug="obsstudio";             File="obs-studio.svg"}
    @{Slug="twitch";                File="twitch.svg"}
    @{Slug="obsidian";              File="obsidian.svg"}
    @{Slug="notion";                File="notion.svg"}
    @{Slug="github";                File="github.svg"}
    @{Slug="git";                   File="git.svg"}
    @{Slug="docker";                File="docker.svg"}
    @{Slug="postman";               File="postman.svg"}
    @{Slug="figma";                 File="figma.svg"}
    @{Slug="windowsterminal";       File="windows-terminal.svg"}
    @{Slug="powershell";            File="powershell.svg"}
    @{Slug="vlcmediaplayer";        File="vlc.svg"}
    @{Slug="gimp";                  File="gimp.svg"}
    @{Slug="inkscape";              File="inkscape.svg"}
    @{Slug="blender";               File="blender.svg"}
    @{Slug="audacity";              File="audacity.svg"}
    @{Slug="thunderbird";           File="thunderbird.svg"}
    @{Slug="libreoffice";           File="libreoffice.svg"}
    @{Slug="dropbox";               File="dropbox.svg"}
    @{Slug="googledrive";           File="google-drive.svg"}
    @{Slug="mega";                  File="mega.svg"}
    @{Slug="7zip";                  File="7zip.svg"}
    @{Slug="filezilla";             File="filezilla.svg"}
    @{Slug="qbittorrent";           File="qbittorrent.svg"}
    @{Slug="bitwarden";             File="bitwarden.svg"}
    @{Slug="nodedotjs";             File="nodejs.svg"}
    @{Slug="python";                File="python.svg"}
    @{Slug="rust";                  File="rust.svg"}
    @{Slug="dotnet";                File="dotnet.svg"}
    @{Slug="transmission";          File="transmission.svg"}
    @{Slug="wireshark";             File="wireshark.svg"}
    @{Slug="mysql";                 File="mysql.svg"}
    @{Slug="redis";                 File="redis.svg"}
    @{Slug="sqlite";                File="sqlite.svg"}
    @{Slug="mongodb";               File="mongodb.svg"}
)

if (-not (Test-Path -LiteralPath $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

$ok = 0
$fail = 0
$total = $apps.Count

Write-Host "=== Descargando $total SVGs desde SimpleIcons ===" -ForegroundColor Cyan

foreach ($app in $apps) {
    $url = "https://cdn.jsdelivr.net/npm/simple-icons@v16/icons/$($app.Slug).svg"
    $filePath = Join-Path -Path $OutputDir -ChildPath $app.File

    Write-Host "[$($ok + $fail + 1)/$total] $($app.Slug) -> $($app.File) ... " -NoNewline

    try {
        Invoke-WebRequest -Uri $url -OutFile $filePath -UseBasicParsing -TimeoutSec 10
        Write-Host "OK" -ForegroundColor Green
        $ok++
    } catch {
        Write-Host "FAIL" -ForegroundColor Red
        $fail++
    }
}

Write-Host "`n=== Resumen: $ok OK, $fail FAIL ===" -ForegroundColor Cyan
Write-Host "SVGs guardados en: $OutputDir"
