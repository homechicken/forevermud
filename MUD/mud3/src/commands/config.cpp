#include <string>
#include <sstream>

#include "config.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** sets the required permission level to execute this command
*/
Config::Config() {
}

/// Destructor
/** Does nothing
*/
Config::~Config() {
}

/// Singleton getter
Config & Config::Instance() {
	static Config instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Config::getName() {
	return "config";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Config::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: config <option> <setting> ...~res" << END;
		s << "  ~br0Config~res lets you change various settings specific to your ";
		s << "connection. Valid options are:" << END;
		s << "    ~b00color~res" << END;
		s << "    ~b00resolution~res" << END;
		s << "    ~b00prompt~res" << END;
		s << "Calling ~br0config~res with an option and no setting will return help ";
		s << "about that configuration option as well as any required or optional arguments.";
	}
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
bool Config::canProcess(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.empty()) {
		return false;
	}

	StringVector args = Utility::stringToVector(txt, " ");

	// we will always have at least 1 in args[] now, since we check for txt.empty earlier

	std::string myArg = Utility::toLower(args[0]);

	if( myArg == "-h"			||
		myArg == "color"		||
		myArg == "resolution"	||
		myArg == "prompt")
	{
		return true;
	}

	return false;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Config::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.empty()) {
		return false;
	}

	StringVector args = Utility::stringToVector(txt, " ");

	if(Utility::iCompare(args[0], "-h")) {
		return help(player);
	}

	std::string option;
	std::string setting;

	if(args.empty()) {
		option = txt;
	} else {
		option = args[0];
	}

	if(args.size() > 1) {
		setting = args[1];
	}

	std::stringstream s;

	if(option == "color") {
		if(setting.empty()) {
			s << "~br0Usage: config color <on|off|help>~res" << END;
			s << "  Turns ANSI color on or off. For help on writing your own ANSI color ";
			s << "sequences, try ~b00config color help~res.";
			player->Write(s.str());
			player->Prompt();
			return true;
		}
		if(setting == "on") {
			if(player->getColorBlindness() == false) {
				player->Write("ANSI color is already on!");
				player->Prompt();
				return false;
			}
			player->setColorBlindness(false);
			player->Write("ANSI color is now on.");
			player->Prompt();
			return true;
		}
		if(setting == "off") {
			if(player->getColorBlindness() == true) {
				player->Write("ANSI color is already off!");
				player->Prompt();
				return false;
			}
			player->setColorBlindness(true);
			player->Write("ANSI color is now off.");
			player->Prompt();
			return true;
		}
		if(setting == "help") {
			s << "  Color tags begin with the tilde (~~) character and are always followed ";
			s << "by three characters representing: attribute type, foreground color, and ";
			s << "background color. Color sequences are escaped with an extra tilde. " << END << END;
			s << "  Attribute types are: n (normal), b (bold), and f (flashing) ";
			s << "Colors are: k (black), r (red), g (green), y (yellow), b (blue), ";
			s << "m (magenta), c (cyan), w (white)." << END << END;
			s <<   "Use 0 (zero) to indicate no change from the current style. Two special ";
			s << "tags also exist: ~~rev (~revreverse video~res), and ~~res (reset). Some examples ";
			s << "are ~byk~~bykFoo~~res~res (bold yellow text on black background), and ";
			s << "~b00~~b00Bar~~res~res (just bold text).";
			player->Write(s.str());
			player->Prompt();
			return true;
		}
		player->Write("Invalid use of ~b00config color~res");
		player->Prompt();
		return false;
	} // option == "color"

	if(option == "resolution") {
		if(setting.empty() || args.size() < 3) {
			s << "~br0Usage: config resolution <x> <y>~res" << END;
			s << "  Use ~b00config resolution~res to set the size of your client window for ";
			s << "the ForeverMUD engine to format some on-the-fly text to fit your screen. The default ";
			s << "is typically 80x24, or ~b00config resolution 80 24~res." << END;
			s << "  Your current resolution is ~b00" << player->getResolutionX() << " x ";
			s << player->getResolutionY() << "~res.";
			player->Write(s.str());
			player->Prompt();
			return true;
		}
		int x = Utility::toInt(setting);
		int y = Utility::toInt(args[2]);

		if(x < glob.Config.getIntValue("ClientScreenFloorX") || y < glob.Config.getIntValue("ClientScreenFloorY")) {
			player->Write("Come on, your resolution isn't ~b00that~res bad, is it?");
			player->Prompt();
			return false;
		}
		player->setResolution(x,y);
		s << "Changing resolution to " << x << "x" << y << ".";
		player->Write(s.str());
		player->Prompt();
		return true;
	}

	if(option == "prompt") {
		if(setting.empty()) {
			s << "~br0Usage: config prompt <text>~res" << END;
			s << "  Use ~b00config prompt~res to change your game prompt, or use ";
			s << "~b00config prompt default~res to reset your prompt to the default value." << END << END;
			s << "  Prompt tokens that will be expanded are:" << END;
			s << "    ~b00%n~res = CRLF" << END;
			s << "    ~b00%r~res = Current Room" << END;
			s << "    ~b00%e~res = Exits from room" << END;
			s << "    ~b00%m~res = My condition" << END;
			s << "    ~b00%t~res = My comfort level" << END;
			std::string prompt = player->getPrompt();
			// we need to escape our color sequences so they don't get parsed on the way
			// out to the client!
			prompt = Utility::stringReplace(prompt, "~", "~~");
			s << "Your current prompt is: " << prompt;
			player->Write(s.str());
			player->Prompt();
			return true;
		}

		if(setting == "default") {
			setting = glob.Config.getStringValue("DefaultPrompt");

			if(setting.empty()) {
				player->setPrompt(setting);
			} else {
				player->setPrompt(":>");
			}
		}

		// if setting has a space, we can't get it this way, so to be sure, we
		// just extract it from txt
		setting = txt.substr(option.length() + 1);
		player->setPrompt(setting);
		player->Write("Prompt changed.");
		player->Prompt();
		return true;
	}

	player->Write("Invalid use of ~b00config~res");
	player->Prompt();
	return false;
}
