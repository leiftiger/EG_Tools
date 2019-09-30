# Using the editor

To begin with, you need to set definition paths for your Evil Genius files using the instructions in the appropriate window. These will load certain definitions directly from the game, including changes made by mods. The editor will work without them, but may make it harder to use and view.

With that out of the way, you need to load a copy of an ATN file, such as `ATNData.ros` or `ATNData_Objects.ros`. `ATNResources.ros` contains necessary resource relationships that the editor uses, but should be loaded automatically in the background from the definition paths. It is also prudent to load all available ATN files when editing as they share global unique IDs, which could potentially conflict if editing only one by one. The ATN editor will use unique IDs starting from the highest in any loaded file for any new additions to the ATN files. Unique IDs are purposedly locked from editing as there should be no reason to edit them manually.

Now that you've loaded a file, you can use the search field to find networks that you want to inspect or edit or you can choose to create a new network in a specific ATN file.

Note that the base game separates overall game logic of objectives into `ATNData.ros` and logic for object interactions in `ATNData_Objects.ros`, it may be wise to follow this structure as it's not certain whether or not all files are considered when referenced in other parts of the game.

With the network open you can now [modify the network](/doc/ATN_structure.md). You can right-click connectors to navigate easier. You can also right-click states & transitions to copy/paste certain structures to ease working in the network. Right-clicking the space between states also allows you to add or paste a new state at that specific position rather than at the end of the network. Other interactions should be intuitive from the interface.

When finished, save all open ATN files and your changes will be applied to the files you've opened. Even though you may not have edited all files, they're all still rewritten by the program - so your mod should only need the file that you actually changed.

# Parameter values

As the editor is unofficial, some parameter type definitions may not be found - these have been labelled as either `MISSING: <NUMBER>` or the raw number directly. The number is a hash digest using FNV1 32-bit, which the game uses to store string references in a cheaper manner. You can use the hash tool included with the editor to manually set the number that a percept or effect may need, and use the same `MISSING: <NUMBER>` format to apply it in the files.

In certain situations, it may also be necessary to pass `NULL` as the argument - this is defined in the editor for all parameter types but may only work in certain cases. Known usage is to pass `NULL` to certain events where it doesn't matter whether or not the event involves the first or second entity. If you want to pass a resource as `NULL` the best practice is to name it `_NULL` and never modify it.

# Best practices

To reduce the risks of mod conflicts, it is encouraged of mods to primarily add new entries, as modifications are more likely to cause conflicts in the Mod Manager.

For instance, when creating a new objective, it would be prudent to use a new thread along with a new state to launch the new network. Adding new transitions, resources and parameters (preferably last in the lists) are likewise non-conflicting modifications to existing entries.
