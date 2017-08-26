@echo off
set PF_DIR=%ProgramFiles%
if defined ProgramFiles(x86) set PF_DIR=%ProgramFiles(x86)%
set PF_DIR=%PF_DIR%\Inno Setup 5
"%PF_DIR%\ISCC.exe" installer_script.iss