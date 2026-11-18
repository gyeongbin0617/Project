@echo off
setlocal
cd /d "%~dp0"
if not exist "NumberExplorers.exe" (
  echo First compile with BUILD_UCRT64.bat using MSYS2 UCRT64 GCC.
  pause
  exit /b 1
)
"NumberExplorers.exe" > "run.log" 2>&1
if errorlevel 1 (
  echo.
  echo The game could not start. Details:
  type "run.log"
  echo.
  echo Keep Assets and all three SDL2 DLL files beside the EXE.
  pause
  exit /b 1
)
