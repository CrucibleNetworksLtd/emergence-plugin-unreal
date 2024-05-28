del /s /f /q %~dp0Binaries\*.*
RMDIR %~dp0Binaries /S /q
del /s /f /q %~dp0Intermediate\*.*
RMDIR %~dp0Intermediate /S /q

for /d %%D in ("%~dp0\Plugins\*") do (
    REM Check if the specific sub-folder exists
    if exist "%%D\Binaries" (
		del /s /f /q %%DBinaries\*.*
        rmdir /s /q "%%D\Binaries"
    )
	if exist "%%D\Intermediate" (
	del /s /f /q %%DIntermediate\*.*
        rmdir /s /q "%%D\Intermediate"
    )
)