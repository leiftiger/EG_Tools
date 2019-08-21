# ATN Structure
The ATN files are hierarchically structured around networks owning states owning transitions that utilize effects and percepts.

## Network
An ATN network can have any number of threads, resources, parameters and states that execute game logic. Networks are essentially Finite State Machines (FSMs), whose starting points begin at the specified threads. Networks need at minimum one thread to function.

A network is only considered to be exited when all threads are terminated.

### Thread
A thread acts as an individual FSM pointer, which keeps track of where it is in the network to know what transitions should be evaluated next. In the ATN files, the threads designate the starting point of each FSM pointer, which during game execution will move away from that state in all possible ways it can before reaching a termination condition.

Threads can in a transition be told to terminate, returning either `true` or `false`. The return value can then be read by a parent network to determine whether or not a network executed properly - it is uncertain what value is returned if multiple threads return different values.

### Resource
A resource is a pass-by-reference object that is initially set to NULL until modified by transition effects.

Resources can be inputs to the network by checking the input checkbox, meaning that they will point to an external reference used in a parent network. The inputs don't necessarily have to be first in the list, but it's a good practice to keep them as such in any case.

Resource types are subclasses of each other as defined in `ATNResources.ros`, such that e.g. a `Character` can be cast as a `CharacterGroup` or a `Resource`.

### Parameter
A parameter is a pass-by-value object that is always set to some value, usually via a subnetwork transition or by the game itself (see: research objects).

Parameter types are all independent of each other and cannot be cast to any other type, and represent different values such as description IDs or animation strings that can be used in network transitions.

Parameters can have a default value set, which is the value initially set when e.g. first setting a network transition and are mainly for making it easier to use the networks.

### State
A state is a point in the FSM that can transition to a subnetwork. If the state has a subnetwork transition, then it will spawn a new subnetwork from that point with specified resources and arguments as input. The subnetwork can either be executed in parallel with the parent network that spawned it, or the parent network can wait for it to finish executing before proceeding using the percept "exited subnetwork". However, the base game's networks always seem to wait for the subnetwork to terminate, so it may be the case that if the parent network terminates, all subnetworks do as well - or worse, possibly crashing the game.

A state also has a number of transitions, that govern where the state should transition to at that point. The first transition that evaluates true in the given transition order is used to go to the next state. If none is true at the current time in the game, the FSM will remain at that state until one is.

#### Transition
A transition is connected to another state, the next state that the FSM pointer will point to if the transition is taken.

The transition is taken according to the percept function used, which determine if the transition can be taken at the current time.

If the transition is taken, it can also perform an effect function - modifying the current game state or interacting with network resources.

To summarize, when a transition can be taken, the effect is executed and then the FSM will now point to the next state the transition is connected to.

# Object networks
If the network is spawned by an object being interacted with by the interaction slot containing the key `ATNNetworkName`, then it will be passed two resources automatically as input by the game. The first resource being the `Character` that interacted with it and the second resource being the `Object` that's being interacted with.

It's important that the order is preserved as it is uncertain how rigid these structures are, as all existing networks seem to require that structure. However, in certain cases, such as loot admiration, the `Object` resource is seemingly not needed, so if the network doesn't use that resource, it doesn't have to include it.

This stucture is used by almost all objects, including training/trainee, stat recharging, tourist interactions, loot and more. Certain exceptions are noted below.

# Research object networks
Similar to object networks, research objects have the character and object being interacted with. To add to that, they also have a research card `Item` as the third input resource, which is the research vial being carried around. In addition, they also have an `Integer` parameter denoting how long the research should take in milliseconds, which should have a default value of 10 000 milliseconds.

# Interrogation object networks
Similar to object networks, the first two `Character` input resources is the guard and the prisoner being interrogated. The third input resource is the `Object` being used to interrogate them with.
