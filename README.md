This is a collection of tools for the 2004 game Evil Genius, which encompasses an ATN Editor and a Mod Manager.

# ATN Editor
![Example image](/doc/images/editor_example.png)
With the [ATN Editor](doc/using_the_editor.md), you are able to edit key logic in Evil Genius, making it possible to:

* Add new henchmen without having to replace any existing ones
* [Create brand new objectives or change existing ones](doc/creating_new_objectives.md)
* [Fix the conference table bug](https://www.moddb.com/games/evil-genius/downloads/objective-2-conference-table-fix)
* Boost minion effects of them interacting with objects
* Disable all tourists, or have even more of them
* [Earn money in the casino and hotel business](https://www.moddb.com/games/evil-genius/downloads/hotel-earnings-mod)
* And much more!

Note that the ATNs only govern parts of the game, you will still have to add string entries or entity descriptions to make full use of it for the use cases above.

# Mod Manager
![Example image](/doc/images/mod_manager_example.png)
With the Mod Manager, you can safely use several mods that partially change the same files (such as the ATN files).

It will automatically scan for the changes made by each mod and try to merge them to a single file without altering the behaviour of the mods. In case of conflicts, it will prioritize the first loaded mod in a user-defined order, such that most desired effects of all mods are included.
