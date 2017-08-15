@echo off
del packages\com.adminst.squidstat\data\data.7z
del SquidstatInstaller.exe
archivegen.exe .\packages\com.adminst.squidstat\data\data.7z .\..\..\..\out\Release\_SquidStat\* .\..\..\..\3rdparty\vc_redist.x86.exe .\..\..\..\3rdparty\vc_redist.x86.install.cmd
binarycreator.exe --offline-only -p .\packages\ -c .\config\config.xml SquidstatInstaller
del .\..\..\..\out\SquidstatInstaller.exe
copy SquidstatInstaller.exe .\..\..\..\out\
del SquidstatInstaller.exe