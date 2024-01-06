@echo off
REM Get current date
For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set mydate=%%b-%%a-%%c)
For /f "tokens=1-2 delims=/:" %%a in ("%TIME%") do (set mytime=%%a-%%b)
set DATETIME=%mydate%_%mytime%

set OUTPUT=c:\alpha_publish\Alpha_%DATETIME%

set OUTPUT_DIR=%USERPROFILE%\Alpha\

set SRC_ROOT=C:\workspace\Alpha\

REM copy the executable
set EXE_SRC=%OUTPUT_DIR%output\Alpha\x64\Release\
xcopy %EXE_SRC%*.exe %OUTPUT%\bin /I /F
xcopy %EXE_SRC%*.cso %OUTPUT%\bin /I /F
xcopy %EXE_SRC%*.ini %OUTPUT%\bin /I /F

REM copy the editor script
xcopy %SRC_ROOT%code\scripts\*.* %OUTPUT%\scripts /I /F

echo Alpha published to %OUTPUT%
