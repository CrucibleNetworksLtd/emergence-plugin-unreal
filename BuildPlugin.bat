rem tar.exe -C  %~dp0\Plugins -a -cf %~dp0\PluginsClean.zip "VRM4U\Content" "VRM4U\Resources" "VRM4U\Source" "VRM4U\ThirdParty" "VRM4U\VRM4U.uplugin" "EmergenceVRMSupport\Resources" "EmergenceVRMSupport\Source" "EmergenceVRMSupport\EmergenceVRMSupport.uplugin" "Emergence\Content" "Emergence\EmergenceServer" "Emergence\Resources" "Emergence\Source" "Emergence\Emergence.uplugin"
"J:\UE_5.1\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="H:\emergence-plugin-unreal\Plugins\Emergence\Emergence.uplugin" -Package="H:\emergence-plugin-unreal\package" -Rocket

pause
