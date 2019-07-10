#include <iostream>
#include "utils.h"

#include <QApplication>

#include "UI/UI_MainWindow.h"
#include "UI/UI_ErrorWindow.h"

int main(int argc, char *argv[])
{
#ifndef _DEBUG
	try
#endif
	{
		/*

		All existing parameter types:

		3D Gui Element			hash
		Act Of Infamy ID		hash
		Agent Group Type				int - from desc id
		Agent Operation Type			int
		Animation Attachment	hash
		Animation Biped Type	  (unused)
		Animation Flags					int flag
		Animation				hash
		Boolean Value					small int
		Camera View ID			hash
		Character Tag					small int
		Dialog Type						int
		Effect return value				int
		Entity Class			hash
		Entity Type						int desc id
		Event					hash
		Floating Graphic		hash
		Game Feature					int flag
		Game Flag				hash
		GUI Control				hash
		Hotspot					hash
		Integer							int
		Interaction Reason				small int
		LockName				hash
		Mouse Button					int - standard mouse button designation (0 left, 1 right)
		Move Flag						int flag
		Network ID						int
		Objective ID			hash
		Region Activity					int
		Research card status			int
		Room Type						int - from desc id
		ScaleType						int
		Sound Category ID		hash
		Sound ID				hash
		Spawn Type						int
		SpecialEffect			hash
		String ID				hash
		Terminate Interaction Priority	small int
		UI State				hash
		Video ID				hash
		World Map Region				int - from desc id
		World Region					int

		*/

		// TODO: more string hashes in above list
		ATN::Manager::setDefinitions("Animation", util::parseHashes("files/anims.txt"));
		ATN::Manager::setDefinitions("Event", util::parseHashes("files/events.txt"));


		ATN::Manager::setDefinitions("Boolean Value", util::createDefinition(
			{
				{"false",	0},
				{"true",	1},
			}));

		// TODO: Confirm
		ATN::Manager::setDefinitions("Character Tag", util::createDefinition(
			{
				{"IGNORE",	0},
				{"KILL",	1},
				{"CAPTURE",	2},
				{"WEAKEN",	3},
			}));

		ATN::Manager::setDefinitions("Effect return value", util::createDefinition(
			{
				{"false",	1},
				{"true",	3},
			}));

		// TODO: Are there more values?
		ATN::Manager::setDefinitions("Mouse Button", util::createDefinition(
			{
				{"LeftClick", 0},
				{"RightClick", 1},
			}));

		// TODO: Confirm
		ATN::Manager::setDefinitions("Research card status", util::createDefinition(
			{
				{"Ordered",			0},
				{"Researching",		2},
				{"NotResearching",	3},
				{"Aborted",			4},
				{"Analyzing",		5},
				{"Complete",		6},
			}));

		ATN::Manager::setDefinitions("ScaleType", util::createDefinition(
			{
				{"Health",		0},
				{"Loyalty",		1},
				{"Smarts",		2},
				{"Attention",	3},
				{"Endurance",	4},
			}));

		// TODO: Are there more values?
		ATN::Manager::setDefinitions("Terminate Interaction Priority", util::createDefinition(
			{
				{"Normal", 0},
				{"Terminate", 1},
			}));

		ATN::Manager::setDefinitions("World Map Region", util::createDefinition(
			{
				{"Antartica",		50000},
				{"Australasia",		50001},
				{"CentralAsia",		50002},
				{"CentralRussia",	50003},
				{"Cuba",			50004},
				{"EastCoast",		50005},
				{"EasternBloc",		50006},
				{"Europe",			50007},
				{"MiddleEast",		50008},
				{"MidWest",			50009},
				{"NorthAfrica",		50010},
				{"NorthChina",		50011},
				{"PacificAllies",	50012},
				{"Polynesia",		50013},
				{"Siberia",			50014},
				{"SouthAfrica",		50015},
				{"SouthAmerica",	50016},
				{"SoutheastAsia",	50017},
				{"Subcontinent",	50018},
				{"WestCoast",		50019},
			}));

		ATN::Manager::setDefinitions("World Region", util::createDefinition(
			{
				{"PATRIOT",	0},
				{"SABRE",	1},
				{"HAMMER",	2},
				{"ANVIL",	3},
				{"SMASH",	4},
			}));

		for (const std::pair<std::string, std::string> &pair : util::parseInterpretations("files/effect_interpretations.txt"))
		{
			ATN::Manager::addInterpretation(pair.first, pair.second);
		}

		for (const std::pair<std::string, std::string> &pair : util::parseInterpretations("files/percept_interpretations.txt"))
		{
			ATN::Manager::addInterpretation(pair.first, pair.second);
		}

		QApplication a(argc, argv);
		UI_MainWindow w;
		w.show();

		return a.exec();
	}
#ifndef _DEBUG
	catch (std::exception &e)
	{
		QApplication a(argc, argv);
		UI_ErrorWindow w;
		w.setErrorMessage(e.what());
		w.show();

		return a.exec();
	}
#endif
}