del /s /f /q "%~dp0Saved\SampleProject\*.*"
RMDIR /s /q "%~dp0Saved\SampleProject"
del /s /f /q "%~dp0Binaries\*.*"
RMDIR "%~dp0Binaries /S /q"
del /s /f /q "%~dp0Intermediate\*.*"
RMDIR "%~dp0Intermediate" /S /q

for /d %%D in ("%~dp0\Plugins\*") do (
    REM Check if the specific sub-folder exists
    if exist "%%D\Binaries" (
		del /s /f /q "%%D\Binaries\*.*"
        rmdir /s /q "%%D\Binaries"
    )
	if exist "%%D\Intermediate" (
	del /s /f /q "%%D\Intermediate\*.*"
        rmdir /s /q "%%D\Intermediate"
    )
)

mkdir "%~dp0Saved\SampleProject"
xcopy /s /v /y /i "%~dp0SampleProjectBase" "%~dp0Saved\SampleProject"
xcopy /s /v /y /i "%~dp0Plugins\EmergenceSamples" "%~dp0Saved\SampleProject\Plugins\EmergenceSamples"
xcopy /s /v /y /i "%~dp0Plugins\VRM4U" "%~dp0Saved\SampleProject\Plugins\VRM4U"
xcopy /s /v /y /i "%~dp0Plugins\Emergence" "%~dp0Saved\SampleProject\Plugins\Emergence"
xcopy /s /v /y /i "%~dp0Plugins\EmergenceVRMSupport" "%~dp0Saved\SampleProject\Plugins\EmergenceVRMSupport"
del "UnrealSampleProject.zip"
tar.exe -C "%~dp0Saved\SampleProject" -cf UnrealSampleProject.zip "Config" "Plugins" "Source" "EmergenceSample.uproject"
start explorer.exe "%~dp0Saved\SampleProject\"
pause