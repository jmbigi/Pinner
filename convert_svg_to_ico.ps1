param(
    [string]$SvgDir = "C:\Desa\Pinner\icons\svg",
    [string]$IcoDir = "C:\Desa\Pinner\icons\ico"
)

$magick = "C:\Program Files\ImageMagick-7.1.1-Q16\magick.exe"

if (-not (Test-Path $IcoDir)) { New-Item -ItemType Directory -Path $IcoDir -Force | Out-Null }

$svgs = Get-ChildItem -Path $SvgDir -Filter "*.svg" -File
$total = $svgs.Count
$ok = 0
$fail = 0

Write-Host "=== Convirtiendo $total SVGs a ICO con ImageMagick ===" -ForegroundColor Cyan

foreach ($svg in $svgs) {
    $icoName = [System.IO.Path]::GetFileNameWithoutExtension($svg.Name) + ".ico"
    $icoPath = Join-Path $IcoDir $icoName

    Write-Host "[$($ok + $fail + 1)/$total] $($svg.Name) -> $icoName ... " -NoNewline

    try {
        $p = Start-Process -FilePath $magick -ArgumentList "convert `"$($svg.FullName)`" -define icon:auto-resize=256,64,48,32,16 `"$icoPath`"" -NoNewWindow -Wait -PassThru
        if ($p.ExitCode -eq 0 -and (Test-Path $icoPath)) {
            Write-Host "OK" -ForegroundColor Green
            $ok++
        } else {
            Write-Host "ERROR (exit: $($p.ExitCode))" -ForegroundColor Red
            $fail++
        }
    } catch {
        Write-Host "FAIL: $_" -ForegroundColor Red
        $fail++
    }
}

Write-Host ""
Write-Host "=== Resumen ===" -ForegroundColor Cyan
Write-Host "  OK:     $ok" -ForegroundColor Green
Write-Host "  FAIL:   $fail" -ForegroundColor Red
Write-Host "  Total:  $total" -ForegroundColor White
Write-Host "  ICO en: $IcoDir"
