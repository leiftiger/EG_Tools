This is a collection of tools for the 2004 game Evil Genius, which encompasses an ATN Editor and a Mod Manager. See [releases](https://github.com/leiftiger/EG_Tools/releases) to download compiled versions for Windows.

# Tools

## ATN Editor
![Example image](doc/images/editor_example.png)
With the [ATN Editor](doc/using_the_editor.md), you are able to edit key logic in Evil Genius, making it possible to:

* Add new henchmen without having to replace any existing ones
* [Create brand new objectives or change existing ones](doc/creating_new_objectives.md)
* [Fix the conference table bug](https://www.moddb.com/games/evil-genius/downloads/objective-2-conference-table-fix)
* Boost minion effects of them interacting with objects
* Disable all tourists, or have even more of them
* [Earn money in the casino and hotel business](https://www.moddb.com/games/evil-genius/downloads/hotel-earnings-mod)
* And much more!

Note that the ATNs only govern parts of the game, you will still have to add string entries or entity descriptions to make full use of it for the use cases above.

## Mod Manager
![Example image](doc/images/mod_manager_example.png)
With the Mod Manager, you can safely use several mods that partially change the same files (such as the ATN files).

It will automatically scan for the changes made by each mod and try to merge them to a single file without altering the behaviour of the mods. In case of conflicts, it will prioritize the first loaded mod in a user-defined order, such that most desired effects of all mods are included.

# Build instructions

## Windows

Solution and project files are provided for compilation with Visual Studio, you only need to install [Qt Visual Studio Tools](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools-19123) to compile the solution. You may need to configure Qt project settings, linking to your installed version of Qt and including the Qt modules "Core", "GUI" and "Widgets". Use `windeployqt` to generate files needed for deployment.

The executables are placed in `/x64/Release/` or `/x64/Debug/` depending on configuration.

## Linux

Run `./build.sh` and everything should build automatically. You may need to have `qtcreator` installed to have all necessary dependencies. For Ubuntu-based distros, this is available via `apt-get install qtcreator`.

The executables are placed in `/x64/Release/` with the extension `.out`.
