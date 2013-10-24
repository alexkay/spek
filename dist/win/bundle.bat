rem This script will build an MSI installer for Win32.
rem Check README.md in this directory for instructions.

set WIX_PATH=c:\Program Files\Windows Installer XML v3.5\bin

cd tests
test.exe
if %errorlevel% neq 0 exit /b %errorlevel%
cd ..

rem Generate a wxs for files in Spek
del spek.msi
move Spek\spek.exe .\
"%WIX_PATH%"\heat dir Spek -gg -ke -srd -cg Files -dr INSTALLLOCATION -template fragment -o files.wxs
move spek.exe Spek\

rem Make the MSI package
"%WIX_PATH%"\candle SpekInstallDirDlg.wxs SpekInstallDir.wxs spek.wxs files.wxs
"%WIX_PATH%"\light -b Spek SpekInstallDirDlg.wixobj SpekInstallDir.wixobj spek.wixobj files.wixobj -ext WixUIExtension.dll -o spek.msi
start /wait fix-msi.js spek.msi

rem Clean up
del files.wxs
del *.wixobj
del *.wixpdb
