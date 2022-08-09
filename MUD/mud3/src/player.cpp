#include "player.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** The constructor sets up some default values of the player object that should
	be overwritten when the actual object is loaded.
*/
Player::Player() {
	mLoginAttempts = 0;
	setObjectType(PlayerObject);
	mPermissionLevel = PlayerPermissions;
	std::string defaultPrompt = glob.Config.getStringValue("DefaultPrompt");
	if(!defaultPrompt.empty()) {
		setPrompt(defaultPrompt);
	} else {
		setPrompt(":>");
	}
}

/// Destructor
/** The destructor removes all ChatChannel and Event subscriptions that were set up
*/
Player::~Player() {
	glob.messageDaemon.leaveAll(getName());
	glob.eventDaemon.clearEventsForTarget(getName());
}

/// Receive messages passed to me
/** This function receives messages from a parent container or daemon and determines
	what should be done with them.
	@param message a Message object being delivered to the player
*/
void Player::processMessage(Message::MessagePointer message) {
	std::stringstream s;
	ObjectLocation loc = getLocation();

	Zone::ZonePointer zone;
	Room::RoomPointer room;

	bool cold = false;

	if(loc.type == RoomObject) {
		zone = glob.zoneDaemon.getZone(loc.zone);

		if(!zone) {
			glob.log.error(boost::format("Player::processMessage(): %1%'s location in zone %2% is not valid") % getName() % loc.zone);
			return;
		}
		room = zone->getRoom(loc.location);

		if(!room) {
			glob.log.error(boost::format("Player::processMessage(): %1%'s location in room %2% (zone %3%) is not valid") % getName() % loc.location % loc.zone);
			return;
		}
	} else {
		/// \todo allow players to be in non-room locations?
		return;
	}

	// this is cool, you see breath if it's cold enough..
	if(room->getTemperature() < 45) {
		cold = true;
	}

	PlayerPointer fromPlayer = glob.playerDatabase.getPlayer(message->getFrom());
	std::string possessive;

	if(fromPlayer) {
		possessive = fromPlayer->getPossessivePronoun();
	}

	switch(message->getType()) {
	case Message::Login:
	case Message::Quit:
		if(Utility::iCompare(message->getFrom(), getName())) {
			// message is from myself, ignore it
			return;
		}

		s << END << message->getBody();
		Write(s.str());

		break;

	case Message::Speech:
		if(Utility::iCompare(message->getFrom(), getName())) {
			s << END << "You say: " << message->getBody();

			if(cold) {
				s << END << "You can see your breath as you talk.";
			}

			Write(s.str());
			return;
		}

		if(fromPlayer) {
			possessive = fromPlayer->getPossessivePronoun();
		}

		if(hasCondition("deaf")) {
			s << END << "You see " << Utility::toProper(message->getFrom()) << "'s mouth ";
			s << "move but you can't hear anything.";

			if(cold) {
				s << " You can, however, see ";

				if(!possessive.empty()) {
					s << possessive;
				} else {
					s << "their";
				}

				s << " breath.";
			}
		} else {
			s << Utility::toProper(message->getFrom()) << " says: " << message->getBody();

			if(cold) {
				s << END << "It's cold enough to see ";

				if(!possessive.empty()) {
					s << possessive;
				} else {
					s << "their";
				}

				s << " breath.";
			}
		}

		Write(s.str());
		resendMessage(message);
		break;

	case Message::Chat:
		s << END << "[~nc0" << message->getHeader() << "~res:";

		if(Utility::iCompare(message->getFrom(), getName())) {
			s << "You";
		} else {
			s << Utility::toProper(message->getFrom());
		}

		s << "] " << message->getBody();
		Write(s.str());
		break;

	case Message::Tell:
		s << END << Utility::toProper(message->getFrom()) << " tells you: ";
		s << message->getBody();

		Write(s.str());
		break;

	case Message::Emote:
		Write("You send an emote");
		Prompt();
		break;

	case Message::Social:
		if(Utility::iCompare(message->getFrom(), getName())) {
			return;
		}

		if(!message->getHeader().empty()) {
			if(Utility::iCompare(message->getHeader(), getName())) {
				// ignore emotes targeted to 'other' group if I'm in the header
				// this means I was the 'target' of the emote, and if I'm in the header,
				// I already received my target message
				return;
			}
		}

		s << END << "[Emote:" << Utility::toProper(message->getFrom()) << "]: " << message->getBody();

		Write(s.str());

		break;

	case Message::Information:
		Write(boost::format("\n[Info:%1%]: %2%") % Utility::toProper(message->getFrom()) % message->getBody());

		resendMessage(message);

		break;

	case Message::EntranceExit:
		if(Utility::iCompare(message->getFrom(), getName())) {
			// message is from myself, ignore it
			return;
		}

		Write(boost::format("\n%1%: %2%") % Utility::toProper(message->getFrom()) % message->getBody());

		break;

	default:
		glob.log.error("Received bad message type!");
	}

	Prompt();
}

/// save the player data
/** This function saves the client data for future retrieval
	\return true if able to save
*/
bool Player::Save() {
	glob.log.debug("Entered Player::Save()");
	YAML::Emitter out;

	out << YAML::BeginMap;
	physicalSave(out);
	glob.log.debug("Saved Physical data");
	playerSave(out);
	glob.log.debug("saved Player data");
	livingSave(out);
	glob.log.debug("saved living data");
	connectionSave(out);
	glob.log.debug("saved connection data");
	sentientSave(out);
	glob.log.debug("saved sentient data");
	containerSave(out);
	glob.log.debug("saved container data");
	out << YAML::EndMap;

	IOResourceLocator resloc;
	resloc.type = getObjectType();
	resloc.name = getName();

	glob.log.debug("Finished with Player::Save()");
	return glob.ioDaemon.saveResource(resloc, out.c_str());
}

/// overridden Save function does nothing
/** This function only exists to satisfy virtual restrictions from the \b Physical class.
	If called, it will log an error. Call the Player::Save() function instead.
	@param out a YAML::Emitter
	\return false all the time.
*/
bool Player::Save(YAML::Emitter &out) const {
	glob.log.error("Player::Save(YAML::Emitter &): A function called me instead of Player::Save()");
	return false;
}

bool Player::Load(const YAML::Node &node) {
	glob.log.error("Player::Load(const YAML::Node &): Called instead of the normal Load() function!");
	return false;
}

/// loads the player data
/** This function loads the player data from storage
	\return true if able to load
*/
bool Player::Load() {
	bool success = false;

	IOResourceLocator resloc;
	resloc.type = getObjectType();
	resloc.name = getName();

	std::string playerFile = glob.ioDaemon.getResource(resloc);

	if(playerFile == IO_RESOURCE_NOT_FOUND) {
		// this is a new player, not an existing one, we can't load it.
		return success;
	}

	std::istringstream is(playerFile);

	try {
		YAML::Parser parser(is);
		YAML::Node doc;

		parser.GetNextDocument(doc);

		success = physicalLoad(doc["Physical"]);

		if(success) {
			success = playerLoad(doc["Player"]);
			glob.log.debug("loaded player node");
		} else {
			glob.log.debug("failed to load physical node");
		}

		if(success) {
			success = livingLoad(doc["Living"]);
			glob.log.debug("loaded living node");
		} else {
			glob.log.debug("failed to load player node");
		}

		if(success) {
			success = connectionLoad(doc["Connection"]);
			glob.log.debug("loaded connection node");
		} else {
			glob.log.debug("failed to load living node");
		}

		if(success) {
			success = sentientLoad(doc["Sentient"]);
			glob.log.debug("loaded sentient node");
		} else {
			glob.log.debug("failed to load connection node");
		}

		if(success) {
			success = containerLoad(doc["Container"]);
			glob.log.debug("loaded container node");
		} else {
			glob.log.debug("failed to load sentient node");
		}

		if(!success) {
			glob.log.debug("failed to load container node");
		}

	} catch(YAML::ParserException &e) {
		std::cout << "YAML Parser exception: " << e.what() << '\n';
		success = false;
	} catch(YAML::BadDereference &e) {
		std::cout << "YAML Bad Dereference: " << e.what() << '\n';
		success = false;
	} catch(YAML::KeyNotFound &e) {
		std::cout << "YAML Map key not found: " << e.what() << '\n';
		success = false;
	} catch(YAML::RepresentationException &e) {
		std::cout << "YAML Representation exception: " << e.what() << '\n';
		success = false;
	} catch(YAML::Exception &e) {
		std::cout << "YAML exception: " << e.what() << '\n';
		success = false;
	} catch(...) {
		std::cout << "Generic exception caught";
		success = false;
	}

	return success;
}

/// Creates a string with all data to save inside
/** This function saves the text that represents the object so it may
	be reloaded at a later time.
	@param out A YAML::Emitter object to send the data to
*/
void Player::playerSave(YAML::Emitter &out) const {
	glob.log.debug("Entered playerSave(YAML::Emitter)");
	out << YAML::Key << "Player" << YAML::Value << YAML::BeginMap;
	glob.log.debug("wrote player key");
	out << YAML::Key << "password" << YAML::Value << mPassword;
	glob.log.debug("wrote password key");
	out << YAML::Key << "prompt" << YAML::Value << mPrompt;
	glob.log.debug("wrote prompt key");
	out << YAML::Key << "permission-level" << YAML::Value << mPermissionLevel;
	glob.log.debug("wrote permission key");
	out << YAML::Key << "aliases" << YAML::Value;
	glob.log.debug("wrote alias key");

	if(mAliases.size() > 0) {
		out << YAML::BeginMap;

		for(StringMap::const_iterator it = mAliases.begin(); it != mAliases.end(); ++it) {
			out << YAML::Key << it->first << YAML::Value << it->second;
			glob.log.debug(boost::format("Saving alias %1% for %2%") % it->first % it->second);
		}

		out << YAML::EndMap;
		glob.log.debug("Finished alias map");
	} else {
		out << YAML::Null;
		glob.log.debug("Wrote NULL value for alias list");
	}

	out << YAML::EndMap;
}

/// loads data from a string
/** This function loads data previously saved into memory
*/
bool Player::playerLoad(const YAML::Node &node) {
	bool success = false;

	glob.log.debug("Loading player node");

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Player::playerLoad(): node is not a map!");
		return success;
	}

	try {
		node["password"] >> mPassword;
		node["prompt"] >> mPrompt;

		/// \todo fix this after players get re-written
		if(const YAML::Node *pName = node.FindValue("description")) {
			*pName >> mDescription;
		}

		glob.log.debug(boost::format("Player's prompt is: %1%") % mPrompt);

		int permission = -1;
		node["permission-level"] >> permission;

		/// \todo should this be moved to Utility namespace?
		switch(permission) {
			case 1:
				mPermissionLevel = AdminPermissions;
				break;
			default:
				mPermissionLevel = PlayerPermissions;
		}

		if(!YAML::IsNull(node["aliases"])) {
			const YAML::Node &aliases = node["aliases"];

			for(YAML::Iterator it = aliases.begin(); it != aliases.end(); ++it) {
				std::string key, value;
				it.first() >> key;
				it.second() >> value;
				addAlias(key, value);
			}
		}
		success = true;
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Player::playerLoad: YAML parser exception caught: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Player::playerLoad: YAML exception caught (no such map key!): %1%") % e.what());
	} catch(...) {
		glob.log.error("Player::playerLoad: caught a fall-through exception");
	}

	glob.log.debug("Done loading player node");

	return success;
}

/// gets an alias for a player command
/** This function checks the player's alias list for a matching command alias
	@param alias the text of the alias
	\return a string the alias should be converted to
*/
std::string Player::getAlias(const std::string &alias) const {
	std::string command = "";

	StringMap::const_iterator it = mAliases.find(alias);

	if(it != mAliases.end()) {
		command = it->second;
	}

	return command;
}

/// generates a list of all aliases
/** This function generates a StringVector list of all aliases currently configured
	for the client.
	\return a StringVector of the player's current aliases
*/
StringVector Player::getAliasList() const {
	StringVector list;

	for(StringMap::const_iterator it = mAliases.begin(); it != mAliases.end(); ++it) {
		std::stringstream s;
		s << it->first << "->" << it->second;
		list.push_back(s.str());
	}

	return list;
}

/// adds an alias for a command
/** This function adds a new alias for a command, replace a current alias if one
	matches
	@param alias the text of the alias
	@param command a string the alias should expand to
*/
void Player::addAlias(const std::string &alias, const std::string &command) {
	removeAlias(alias);
	mAliases[alias] = command;
}

/// removes an alias for a command
/** This function removes an alias for a command if one exists
	@param alias the text of the alias
*/
void Player::removeAlias(const std::string &alias) {
	StringMap::iterator it = mAliases.find(alias);

	if(it != mAliases.end()) {
		mAliases.erase(it);
	}
}

/// Sends a prompt to the player
/** This function behaves almost exactly like ANSI color parsing in ClientSocket
*/
void Player::Prompt() {
	std::stringstream s;
	std::string::size_type pos;

	// we always want the prompt to start on a new line
	s << END;

	for(pos = 0; pos < mPrompt.length(); ++pos) {
		if(mPrompt[pos] != '%') {
			s << mPrompt[pos];
		} else {
			pos += convertPromptToken(&mPrompt[pos], s);
		}
	}
	Write(s.str());
}

/// expands a prompt token to the proper text
/** This function takes a prompt token and replaces it in the outgoing text stream with
	the text that should be in its place
	@param txt the start of the prompt token
	@param[out] out the outgoing stream object
	\return the number of characters that were in the token that were replaced in the outgoing stream
*/
std::string::size_type Player::convertPromptToken(const char *txt, std::stringstream &out) {
	// make sure we aren't escaping the token %
	if(txt[1] == '%') {
		out << txt[1];
		// we return 1 so we don't get double %'s written out
		return 1;
	}
	// declare these in case we need them in the switch statement
	ObjectLocation location = getLocation();

	Zone::ZonePointer zone;
	Room::RoomPointer room;

	if(location.type == RoomObject) {
		zone = glob.zoneDaemon.getZone(location.zone);

		if(!zone) {
			glob.log.error(boost::format("Player::convertPromptToken(): Cannot load player %1%'s zone %2%") % getName() % location.zone);
			return 0;
		}

		room = zone->getRoom(location.location);

		if(!room) {
			glob.log.error(boost::format("Player::converPromptToken(): Cannot load player %1%'s room %2% in zone %3%") % getName() % location.location % location.zone);
			return 0;
		}
	}

	// get our current temperature
	int temp = getTemperature();

	switch(txt[1]) { // we could use tolower(txt[1]) here, but reserve caps for other functions
	case 'n':
		out << END;
		break;

	case 'r':
		out << room->getName();
		break;

	case 'e':
		out << room->getExitString();
		break;

	case 'm':
		out << livingGetStateString();
		break;

	case 't':
		if(temp > getTempDamageHigh()) {
			out << "~bwrBlazing~res";
		} else if(temp > getTempComfortHigh()) {
			out << "~nr0Hot~res";
		} else if(temp >= getTempComfortLow()) {
			out << "Comfortable";
		} else if(temp > getTempDamageLow()) {
			out << "~nb0Cold~res";
		} else {
			out << "~bwbFreezing~res";
		}
		break;

	default:
		// unrecognized prompt, skip the token marker only
		return 0;
	}

	return 1;
}

/// runs regular checks
/** This function checks different variables that may be in flux and acts accordingly.
*/
void Player::heartbeat() {
	// check temperature
	int temp = getTemperature();

	if(temp > getTempDamageHigh()) {
		subtractLife(1);
		Write("Ouch, it's really hot!");
		Prompt();
	} else if(temp < getTempDamageLow()) {
		subtractLife(1);
		Write("Ouch, it's too cold here!");
		Prompt();
	}
}

/// gets a string describing the current status of the player
/** This function generates a string that contains data about most of the internal
	variables that make up a player.
	\return a string description
*/
std::string Player::getStatusString() const {
	std::stringstream s;

	s << "Height: " << getHeight() << END;
	s << "Weight: " << getWeight() << END;
	s << "Temperature: " << getTemperature() << END;
	s << "Conditions: ";

	StringVector conditions = getConditions();

	for(StringVector::const_iterator it = conditions.begin(); it != conditions.end(); ++it) {
		s << *it << " ";
	}

	s << END;

	s << "Life: " << getLife() << " / " << getMaxLife() << END;
	s << "Sex: ";

	switch(getSex()) {
	case 0:
		s << "Male";
		break;
	case 1:
		s << "Female";
		break;
	case 2: // fall through
	default:
		s << "Neuter";
		break;
	}

	s << END;

	s << "State: " << livingGetStateString() << END;
	s << "Posture: " << livingGetPostureString() << END;
	s << "Comfort Temperature Lo/Hi: " << getTempComfortLow() << " / " << getTempComfortHigh() << END;
	s << "Damage Temperature Lo/Hi: " << getTempDamageLow() << " / " << getTempDamageHigh() << END;

	s << "Str: " << getStrength() << END;
	s << "Int: " << getIntelligence() << END;
	s << "Wis: " << getWisdom() << END;
	s << "Dex: " << getDexterity() << END;
	s << "Con: " << getConstitution() << END;
	s << "Cha: " << getCharisma() << END;

	s << "You can carry " << containerGetCapacity() << " objects.";

	return s.str();
}

/// returns the brief description of the player
/** This function generates a dynamic description of the player, describing their stance and
	state of being
	\return the brief description
*/
std::string Player::getBrief() const {
	std::stringstream s;

	s << Utility::toProper(getName()) << " is ";

	switch(livingGetState()) {
		case Living::Alive:
			switch(livingGetPosture()) {
				case Living::Standing: s << "standing "; break;
				case Living::Sitting: s << "sitting "; break;
				case Living::Kneeling: s << "kneeling "; break;
				case Living::Lying: s << "lying "; break;
				case Living::Prone: s << "lying prone "; break;
				default: glob.log.warn("Container found an unrecognized Living::Posture");
			}

			s << "here" << std::endl;
			break;

		case Living::Unconscious:
			s << "here, unconscious" << std::endl;
			break;

		case Living::Dead:
			s << "here, taking a dirt nap" << std::endl;
			break;

		case Living::Ghost:
			s << "a semi-transparent ghost" << std::endl;
			break;

		default:
			glob.log.warn("Player::getBrief(): Found an uninitialized Living::LivingState");
	}

	return s.str();
}

/// gets the verbose description
/** This function (for now) just returns the brief description
*/
std::string Player::getVerbose() const {
	return mDescription;
}

