@echo off
::Keil执行文件位置
set UV=D:\Program\Keil529\UV4\UV4.exe
::查找uvprojx工程文件
for /f "usebackq delims=" %%j in (`dir /s /b %cd%\*.uvprojx`) do (
if exist %%j (
set UV_PRO_PATH="%%j"))
echo ---------------------------------------------------------------
echo Author:Admin
echo Init building ...
echo >build_log.txt
%UV% -j0 -b %UV_PRO_PATH% -l %cd%\build_log.txt
type build_log.txt
::创建Firmwares文件夹，拷贝hex、bin文件
@REM if not exist %cd%\Firmwares md %cd%\Firmwares
@REM del /f /s /q %cd%\Firmwares\*.*
@REM copy %cd%\Output\*.hex %cd%\Firmwares\
@REM copy %cd%\Output\*.bin %cd%\Firmwares\
@REM echo Done.
@REM pause
