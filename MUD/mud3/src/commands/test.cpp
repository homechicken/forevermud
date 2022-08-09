#include <string>
#include <sstream>

#include "test.h"

/// Constructor
/** Does nothing
*/
Test::Test() {
}

/// Destructor
/** Does nothing
*/
Test::~Test() {
}

/// Singleton getter
Test & Test::Instance() {
	static Test instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Test::getName() {
	return "test";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Test::help(Player::PlayerPointer player) {
	std::stringstream s;

	s << "~br0Usage: test~res" << END;
	s << "  ~br0test~res generates a string of characters and ANSI codes.";

	player->Write(s.str());
	player->Prompt();
	return true;
}

/// checks to see if the command works with the arguments provided
/** This command evaluates the arguments and decides whether or not process()
	can be called correctly. If not, the CommandHandler calls the help() function.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command will run properly
*/
bool Test::canProcess(Player::PlayerPointer player, const std::string &txt) {
	return true;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Test::process(Player::PlayerPointer player, const std::string &txt) {
	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}
	std::stringstream s;
	s << "000000000 111111111 222222222 333333333 444444444 555555555 666666666 77777" << END;
	s << "123456789 123456789 123456789 123456789 123456789 123456789 123456789 12345" << END;

	s << "Text Decoration: ~n00Normal ~b00Bold~res ~d00Dim~res ~u00Underscore~res ~f00Blink~res ~revReverse~res" << END;
	s << "Text Decoration: ~nr0Normal ~br0Bold~res ~dr0Dim~res ~ur0Underscore~res ~fr0Blink~res ~nr0~revReverse~res" << END;
	s << "Text Decoration: ~ng0Normal ~bg0Bold~res ~dg0Dim~res ~ug0Underscore~res ~fg0Blink~res ~ng0~revReverse~res" << END;
	s << "Text Decoration: ~ny0Normal ~by0Bold~res ~dy0Dim~res ~uy0Underscore~res ~fy0Blink~res ~ny0~revReverse~res" << END;
	s << "Text Decoration: ~nb0Normal ~bb0Bold~res ~db0Dim~res ~ub0Underscore~res ~fb0Blink~res ~nb0~revReverse~res" << END;
	s << "Text Decoration: ~nm0Normal ~bm0Bold~res ~dm0Dim~res ~um0Underscore~res ~fm0Blink~res ~nm0~revReverse~res" << END;
	s << "Text Decoration: ~nc0Normal ~bc0Bold~res ~dc0Dim~res ~uc0Underscore~res ~fc0Blink~res ~nc0~revReverse~res" << END;
	s << "ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ";
	s << "ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab ab";
	player->Write(s.str());
	player->Prompt();
	return true;
}
