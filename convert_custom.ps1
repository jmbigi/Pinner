$names = @("geologiagb","desviaciones","agrupamiento","migracionespacial","sonido","colores","frmdup")
$magick = "C:\Program Files\ImageMagick-7.1.1-Q16\magick.exe"
foreach ($n in $names) {
  $svg = "C:\Desa\Pinner\icons\svg\$n.svg"
  $ico = "C:\Desa\Pinner\icons\ico\$n.ico"
  & $magick convert $svg -define icon:auto-resize=256,64,48,32,16 $ico
  if ($LASTEXITCODE -eq 0) {
    $s = (Get-Item $ico).Length
    Write-Host "OK $n.ico ($s bytes)"
  } else {
    Write-Host "FAIL $n.ico"
  }
}
