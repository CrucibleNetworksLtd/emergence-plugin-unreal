rmdir %~dp0plugins\Emergence\Binaries /s /q
rmdir %~dp0plugins\Emergence\Intermediate /s /q
E:/UE_4.26/Engine/Binaries/DotNET/UnrealBuildTool.exe  -Module=Emergence EmergenceUEExampleEditor Win64 Development -Project="H:/emergence-plugin-unreal/EmergenceUEExample.uproject" "H:/emergence-plugin-unreal/EmergenceUEExample.uproject"  -IgnoreJunk
E:/UE_4.26/Engine/Binaries/DotNET/UnrealBuildTool.exe  -Module=EmergenceEditor EmergenceUEExampleEditor Win64 Development -Project="H:/emergence-plugin-unreal/EmergenceUEExample.uproject" "H:/emergence-plugin-unreal/EmergenceUEExample.uproject"  -IgnoreJunk
E:/UE_4.26/Engine/Binaries/DotNET/UnrealBuildTool.exe  -Module=EmergenceToolbar EmergenceUEExampleEditor Win64 Development -Project="H:/emergence-plugin-unreal/EmergenceUEExample.uproject" "H:/emergence-plugin-unreal/EmergenceUEExample.uproject"  -IgnoreJunk
rmdir D:\EmergenceBuild /s /q
robocopy %~dp0plugins\Emergence D:\EmergenceBuild\Emergence /E /XD "%~dp0plugins\Emergence\Intermediate" /XD "%~dp0plugins\Emergence\Source\Emergence\Private" /XD "%~dp0plugins\Emergence\Source\EmergenceEditor\Private" /XD "%~dp0plugins\Emergence\Source\EmergenceToolbar\Private"
cd /d D:\EmergenceBuild\
tar -a -c -f D:\EmergenceBuild\Emergence.zip Emergence
pause