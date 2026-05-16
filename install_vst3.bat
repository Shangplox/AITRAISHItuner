@echo off
echo ============================================
echo   AITRAISHItuner VST3 Installer
echo ============================================
echo.
echo Copying VST3 to system folder...
xcopy /E /I /Y "C:\Project\AITRAISHItuner\build\release\AITRAISHItuner_artefacts\Release\VST3\AITRAISHItuner.vst3" "C:\Program Files\Common Files\VST3\AITRAISHItuner.vst3\"
echo.
if %ERRORLEVEL% EQU 0 (
    echo SUCCESS: VST3 installed!
    echo Now open FL Studio and rescan plugins.
) else (
    echo FAILED: Make sure to run this as Administrator.
)
echo.
pause
