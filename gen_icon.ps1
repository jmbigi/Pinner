# Genera src\default.ico (32x32 PNG-compressed .ico)
Add-Type -AssemblyName System.Drawing

$bmp = New-Object System.Drawing.Bitmap(32, 32)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.Clear([System.Drawing.Color]::FromArgb(0, 20, 80))
$font = [System.Drawing.Font]::new("Segoe UI", 20, [System.Drawing.FontStyle]::Bold)
$g.DrawString("P", $font, [System.Drawing.Brushes]::White, 4, 2)
$g.Dispose()

$ms = New-Object System.IO.MemoryStream
$bmp.Save($ms, [System.Drawing.Imaging.ImageFormat]::Png)
$pngBytes = $ms.ToArray()
$ms.Close()
$bmp.Dispose()

$fs = [System.IO.File]::OpenWrite("src\default.ico")
$bw = New-Object System.IO.BinaryWriter($fs)
$bw.Write([byte]0); $bw.Write([byte]0)     # reserved
$bw.Write([byte]1); $bw.Write([byte]0)     # ICO type=1
$bw.Write([byte]1); $bw.Write([byte]0)     # count=1
$bw.Write([byte]32)                         # width
$bw.Write([byte]32)                         # height
$bw.Write([byte]0)                          # color count
$bw.Write([byte]0)                          # reserved
$bw.Write([int16]1)                         # planes
$bw.Write([int16]32)                        # bpp
$bw.Write($pngBytes.Length)                 # size
$bw.Write(22)                               # offset (ico header=6 + dir entry=16 = 22)
$bw.Write($pngBytes)
$bw.Close()
$fs.Close()

Write-Host "  OK: src\default.ico" -ForegroundColor Green
