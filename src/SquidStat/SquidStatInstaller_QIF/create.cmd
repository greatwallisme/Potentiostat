@echo off
IF EXIST packages\com.adminst.squidstat\data\data.7z del packages\com.adminst.squidstat\data\data.7z
IF EXIST SquidstatInstaller.exe del SquidstatInstaller.exe
echo Archiving payload (may take couple minutes)...
archivegen.exe .\packages\com.adminst.squidstat\data\data.7z .\..\..\..\out\Release\_SquidStat\*
echo Creating installer...
binarycreator.exe --offline-only -p .\packages\ -c .\config\config.xml SquidstatInstaller
IF EXIST .\..\..\..\out\SquidstatInstaller.exe del .\..\..\..\out\SquidstatInstaller.exe
IF EXIST SquidstatInstaller.exe (
	copy SquidstatInstaller.exe .\..\..\..\out\
) else (
	echo Error during installer creation!
	exit 1
)
IF EXIST SquidstatInstaller.exe del SquidstatInstaller.exe
echo Installer created!