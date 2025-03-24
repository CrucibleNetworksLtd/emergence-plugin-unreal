# Emergence Unreal Plugin

This is the working repository for the [Emergence Unreal Plugin](https://github.com/CrucibleNetworksLtd/EmergenceSDKUnreal/releases), you can find releases [here in the release repo](https://github.com/CrucibleNetworksLtd/EmergenceSDKUnreal/releases).

## Getting started
To run and compile this, you need the following:
- Unreal Engine 5.4, 5.5
- Visual Studio 2022
- To use the EVM DLL features (only on windows), you need the dotnet runtime install from here https://dotnet.microsoft.com/en-us/download/dotnet/7.0 windows x64

### Steps to running
1. Run the "EmergenceUEExample.uproject" in Unreal Engine.

### Steps to pulling changes
1. Make sure your editor is not open (it likes to hold handles on files).
2. Pull any changes.
3. Open the editor, try to avoid opening any blueprints.
4. Press compile and wait for it to finish.
5. Restart the editor.
6. Done!

## The project's layout
You may notice that we have content and source code in both this game module and in a plugin's modules. Any content which we plan to ship should go in the plugin's content folder via unreal engine. Any content which shouldn't be shipped (e.g. images of designs from the design team your using as a template, etc) - the plugin should never rely on content which is in the content folder. Any source code / testing code should go in the Emergence source folder, there isn't really a reason why you'll need to write source code into the game module's source folder. 

## Useful commands
Recompile the c++ files without the editor needing to be open:
```
[PATH TO UNREAL ENGINE]/Engine/Binaries/DotNET/UnrealBuildTool.exe Development Win64 -project="[PATH TO PROJECT FOLDER]/EmergenceUEExample.uproject" -TargetType=Editor -Progress -NoEngineChanges -NoHotReloadFromIDE
```

Build the repo into a "game":
```
call "[PATH TO UNREAL ENGINE]\Engine\Build\BatchFiles\RunUAT.bat" -ScriptsForProject="[PATH TO PROJECT FOLDER]/EmergenceUEExample.uproject" BuildCookRun -fileopenlog -installed -nop4 -project="[PATH TO PROJECT FOLDER]/EmergenceUEExample.uproject" -iterate -cook -stage -archive -archivedirectory="[OUTPUT DIRECTORY]" -package -clientconfig=Shipping -ue4exe=UE4Editor-Cmd.exe -compressed -pak -prereqs -nodebuginfo -targetplatform=Win64 -build -utf8output
```
