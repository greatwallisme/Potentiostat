@echo off
IF EXIST packages\com.adminst.squidstat\data\data.7z del packages\com.adminst.squidstat\data\data.7z
IF EXIST SquidStatInstaller.exe del SquidStatInstaller.exe
echo Archiving payload (may take couple minutes)...
archivegen.exe .\packages\com.adminst.squidstat\data\data.7z .\..\..\..\out\Release\_SquidStat\* .\..\..\..\3rdparty\vc_redist.x86.exe
echo Creating installer...
binarycreator.exe --offline-only -p .\packages\ -c .\config\config.xml SquidStatInstaller
IF EXIST .\..\..\..\out\SquidStatInstaller.exe del .\..\..\..\out\SquidStatInstaller.exe
IF EXIST SquidStatInstaller.exe (
	copy SquidStatInstaller.exe .\..\..\..\out\
) else (
	echo Error during installer creation!
	exit 1
)
IF EXIST SquidStatInstaller.exe del SquidStatInstaller.exe
echo Installer created!