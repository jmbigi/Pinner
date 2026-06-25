param(
    [string]$SvgDir = ".\icons\svg",
    [string]$IcoDir = ".\icons\ico"
)

$names = @("geologiagb","desviaciones","agrupamiento","migracionespacial","sonido","colores","frmdup")

# Detect ImageMagick in common locations
$magickCandidates = @(
    "magick.exe",
    "${env:ProgramFiles}\ImageMagick-*\magick.exe",
    "${env:ProgramFiles}\ImageMagick-*\convert.exe"
)
$magick = $null
foreach ($candidate in $magickCandidates) {
    $resolved = Resolve-Path $candidate -ErrorAction SilentlyContinue
    if ($resolved) { $magick = $resolved[-1].Path; break }
}
if (-not $magick) { Write-Host "ERROR: ImageMagick no encontrado. Instalalo o pasa la ruta en `$magick"; exit 1 }

$SvgDir = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($SvgDir)
$IcoDir = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($IcoDir)
if (-not (Test-Path $IcoDir)) { New-Item -ItemType Directory -Path $IcoDir -Force | Out-Null }

foreach ($n in $names) {
  $svg = Join-Path $SvgDir "$n.svg"
  $ico = Join-Path $IcoDir "$n.ico"
  & $magick convert $svg -define icon:auto-resize=256,64,48,32,16 $ico
  if ($LASTEXITCODE -eq 0) {
    $s = (Get-Item $ico).Length
    Write-Host "OK $n.ico ($s bytes)"
  } else {
    Write-Host "FAIL $n.ico"
  }
}
