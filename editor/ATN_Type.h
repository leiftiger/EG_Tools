#pragma once

namespace ATN
{
	enum ParameterMarshallType : int
	{
		Unknown = 0,
		Constant = 2,
		Number = 3,				// not sure
		AnotherNumber = 4		// ???
	};

	enum ResourceMarshallType : int
	{
		Unknown = 0,
		ResourceIndex = 11		// Access the network resource at specified index
	};

	enum ResourceType : int
	{
		Unknown = 0,
		Character = 5,			// i.e. genius, minions, agents, henchmen
		Object = 24,
		Number = 26,
		Timer = 27,
		Item = 6634,			// i.e. weapons, briefcases
		EntityGroup = 8927,
		CharacterGroup = 8949
	};
}