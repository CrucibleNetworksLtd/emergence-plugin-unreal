# Emergence Unreal Plugin

## Getting started
To run and compile this, you need the following:
- Unreal Engine 4.26.2
- Visual Studio 2019
- A built version of the Emergence EVM server from [this repo](https://github.com/Crucible-wtd/emergence-evm-server). 

### Steps to running
1. Build the Emergence EVM server as Debug, Any CPU, WalletConnectPoC. 
2. It should have built to the folder `emergence-evm-server\bin\Debug\net5.0`. Copy and paste the entire content of this folder to `emergence-plugin-unreal\Plugins\Emergence\EmergenceServer`.
2. Run the "EmergenceUEExample.uproject" in Unreal Engine.

## The project's layout
You may notice that we have content and source code in both this game module and in a plugin's modules. Any content which we plan to ship (textures used by Emergence) should go in the plugin's content folder via unreal engine. Any content which shouldn't be shipped (e.g. images of designs from the design team your using as a template, etc) - the plugin should never rely on content which is in the content folder. Any source code / testing code should go in the Emergence source folder, there isn't really a reason why you'll need to write source code into the game module's source folder. 

## The plugin's layout
Everything which is code which would run in a game or as part of a testing suite (so pretty much everything) is in the "Emergence" module of the plugin. The plugin's "EmergenceEditor" module contains any code which alters Unreal Engine's UI and must be loaded differently (see the .uproject file for more details) - as of right now this only contains the additions to the project settings menu.

### UI Architecture
Here is the basics of how our UI system (sometimes called the "Agent") is laid out. The game developer is going to assign a button within their game that displays the main widget called `EmergenceUI_BP` (you can see an example of how this might be implemented in `ThirdPersonCharacter` on the "Z" key). When this loads it checks if the localserver has a wallet connect session and then displays either the `HomeScreen` or the `WelcomeScreen`. This widget has: 
- a main slot, which can contain one of many "screens" - screens should be of the class `EmergenceScreen` - it has some utlities that you'll want if you wanna go between screens, etc. 
- a "Topbar" (which is visible on any screen but sometimes changes its appearance depending on the screen / state of the local server's connection).
- a confirmation message, which works a bit like how a toast message works in an android app UI (e.g. screens call a method on EmergenceUI to activate the message, rather than the screen itself containing the message).

