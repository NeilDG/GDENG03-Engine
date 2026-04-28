# Automated Toggle Test Script for Phase 3 Step 11 Validation
# This script will:
# 1. Start the engine
# 2. Wait for initialization (5 seconds)
# 3. Toggle through scenes (SPACE key)
# 4. Toggle deferred rendering (D key)
# 5. Toggle G-Buffer debug (G key)
# 6. Capture frames for analysis

param(
    [int]$TestDurationSeconds = 60
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Phase 3 Step 11 - Automated Toggle Test" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Test Plan:" -ForegroundColor Yellow
Write-Host "  0-10s:  Scene 1 - Forward Rendering" -ForegroundColor Gray
Write-Host "  10-20s: Scene 2 - Forward Rendering" -ForegroundColor Gray
Write-Host "  20-30s: Scene 2 - Deferred Rendering (Toggle D)" -ForegroundColor Gray
Write-Host "  30-40s: Scene 3 - Deferred Rendering" -ForegroundColor Gray
Write-Host "  40-50s: Scene 3 - G-Buffer Debug (Toggle G)" -ForegroundColor Gray
Write-Host "  50-60s: Scene 4 - Deferred Rendering (Toggle G off)" -ForegroundColor Gray
Write-Host ""

# Add Windows Forms assembly for sending keys
Add-Type -AssemblyName System.Windows.Forms

Write-Host "Starting AnitoEngine..." -ForegroundColor Green
$process = Start-Process -FilePath ".\build\bin\Release\AnitoEngine.exe" `
                         -WorkingDirectory (Get-Location).Path `
                         -PassThru

Start-Sleep -Seconds 3
Write-Host "Engine started. PID: $($process.Id)" -ForegroundColor Green

# Helper function to send key to window
function Send-KeyToProcess {
    param([string]$Key)

    # Try to bring window to foreground
    $sig = @'
    [DllImport("user32.dll")]
    public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")]
    public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
'@
    $type = Add-Type -MemberDefinition $sig -Name WindowAPI -PassThru
    $hwnd = $process.MainWindowHandle

    if ($hwnd -ne [IntPtr]::Zero) {
        [void]$type::ShowWindow($hwnd, 5)  # SW_SHOW
        [void]$type::SetForegroundWindow($hwnd)
        Start-Sleep -Milliseconds 200
        [System.Windows.Forms.SendKeys]::SendWait($Key)
        Write-Host "  ✓ Sent key: $Key" -ForegroundColor Cyan
    } else {
        Write-Host "  ⚠ Could not find window handle" -ForegroundColor Yellow
    }
}

try {
    Write-Host "`n[0-10s] Testing Scene 1 - Forward Rendering..." -ForegroundColor Yellow
    Start-Sleep -Seconds 10

    Write-Host "[10s] Switching to Scene 2..." -ForegroundColor Yellow
    Send-KeyToProcess " "  # SPACE key
    Start-Sleep -Seconds 10

    Write-Host "[20s] Toggling Deferred Rendering (D key)..." -ForegroundColor Yellow
    Send-KeyToProcess "d"
    Start-Sleep -Seconds 10

    Write-Host "[30s] Switching to Scene 3..." -ForegroundColor Yellow
    Send-KeyToProcess " "  # SPACE key
    Start-Sleep -Seconds 10

    Write-Host "[40s] Toggling G-Buffer Debug (G key)..." -ForegroundColor Yellow
    Send-KeyToProcess "g"
    Start-Sleep -Seconds 10

    Write-Host "[50s] Toggling G-Buffer Debug OFF and switch to Scene 4..." -ForegroundColor Yellow
    Send-KeyToProcess "g"
    Start-Sleep -Seconds 2
    Send-KeyToProcess " "  # SPACE key
    Start-Sleep -Seconds 8

    Write-Host "`nWaiting for engine to auto-exit..." -ForegroundColor Gray
    $process.WaitForExit()

} catch {
    Write-Host "Error during test: $_" -ForegroundColor Red
} finally {
    if (!$process.HasExited) {
        Write-Host "Stopping engine process..." -ForegroundColor Yellow
        $process.Kill()
        $process.WaitForExit()
    }
}

Write-Host "`n========================================" -ForegroundColor Green
Write-Host "Test Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Results saved to:" -ForegroundColor Cyan
Write-Host "  - Frames: anito-debug/frames/" -ForegroundColor Gray
Write-Host "  - Logs:   anito-debug/logs/" -ForegroundColor Gray
Write-Host "  - Profiling: anito-debug/profiling/" -ForegroundColor Gray
