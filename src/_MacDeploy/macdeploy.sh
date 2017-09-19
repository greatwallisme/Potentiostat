#!/bin/sh
QTDIR="$1"
cp ./../../3rdparty/qtcsv-out/lib-release/libqtcsv.dylib ./../../out/Release/_SquidStat/_SquidStat.app/Contents/MacOS/libqtcsv.1.dylib
cp ./../../3rdparty/hidapi-out/lib-release/libhidapi.0.dylib ./../../out/Release/_SquidStat/_SquidStat.app/Contents/MacOS/
mkdir -p ./../../out/Release/_SquidStat/_SquidStat.app/Contents/Frameworks/qwt.framework/
cp -R ./../../3rdparty/qwt/bin/lib/qwt.framework/ ./../../out/Release/_SquidStat/_SquidStat.app/Contents/Frameworks/qwt.framework/
install_name_tool -change /usr/local/opt/hidapi/lib/libhidapi.0.dylib @executable_path/libhidapi.0.dylib ./../../out/Release/_SquidStat/_SquidStat.app/Contents/MacOS/_SquidStat
install_name_tool -change libqtcsv.1.dylib @executable_path/libqtcsv.1.dylib ./../../out/Release/_SquidStat/_SquidStat.app/Contents/MacOS/_SquidStat
install_name_tool -change qwt.framework/Versions/6/qwt @executable_path/../Frameworks/qwt.framework/Versions/6/qwt ./../../out/Release/_SquidStat/_SquidStat.app/Contents/MacOS/_SquidStat
mkdir -p ./../../out/Release/_SquidStat/_SquidStat.app/Contents/MacOS/prebuilt/
cp -R ./../../out/Release/prebuilt/ ./../../out/Release/_SquidStat/_SquidStat.app/Contents/MacOS/prebuilt/
mkdir -p ./../../out/Release/_SquidStat/_SquidStat.app/Contents/MacOS/elements/
cp -R ./../../out/Release/elements/ ./../../out/Release/_SquidStat/_SquidStat.app/Contents/MacOS/elements/
rm -f ./../../out/Release/_SquidStat/_SquidStat.dmg
$QTDIR/macdeployqt ./../../out/Release/_SquidStat/_SquidStat.app -dmg
