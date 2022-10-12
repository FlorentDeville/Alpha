@echo off
REM Get current date
For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set mydate=%%b-%%a-%%c)
For /f "tokens=1-2 delims=/:" %%a in ("%TIME%") do (set mytime=%%a-%%b)
set DATETIME=%mydate%_%mytime%

set OUTPUT=c:\workspace\Alpha\publish\Alpha_%DATETIME%

set SRC_ROOT=c:\workspace\Alpha\
REM copy the executable
set EXE_SRC=%SRC_ROOT%code\x64\Release\
xcopy %EXE_SRC%*.exe %OUTPUT%\bin /I /F
xcopy %EXE_SRC%*.cso %OUTPUT%\bin /I /F
xcopy %EXE_SRC%*.ini %OUTPUT%\bin /I /F

REM copy the editor script
xcopy %SRC_ROOT%code\scripts\*.* %OUTPUT%\scripts /I /F

REM copy the data and raw
xcopy %SRC_ROOT%data %OUTPUT%\data /I /F /S
xcopy %SRC_ROOT%raw %OUTPUT%\raw /I /F /S

REM copy the editor data
xcopy %SRC_ROOT%editor %OUTPUT%\editor /I /F /S

echo Alpha published to %OUTPUT%
