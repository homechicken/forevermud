#include <string>
#include <sstream>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#include "get.h"
#include "collection.h"
#include "utility.h"

#include "global.h"
extern Global glob;

namespace qi = boost::spirit::qi;

/// Constructor
/** sets the required permission level to execute this command
*/
Get::Get() {
	mMinimumPermissionLevel = Player::PlayerPermissions;
}

/// Destructor
/** Does nothing
*/
Get::~Get() {
}

/// Singleton getter
Get & Get::Instance() {
	static Get get;
	return get;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Get::getName() {
	return "get";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Get::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: get [quantity] <object> [from <object> [container number] ]~res" << END;
		s << "  ~br0Get~res lets you retrieve an item. You may be subject to size ";
		s << "or weigth constraints, though. You can't pick up everything." << END;
		s << "A number preceding the item denotes quantity, a number after the item or container will ";
		s << "differentiate similar items/containers" << END;
		s << "~b00Example commands:~res " ;
		s << "get book, get book 2, get 30 coins, get book from shelf, get book 3 from shelf, get book from shelf 2, ";
		s << "get book 2 from shelf 3, get 30 coins from bag 2, get 5 arrows 2 from quiver 3";
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
bool Get::canProcess(Player::PlayerPointer player, const std::string &txt) {
	bool allowed = false;

	if(player->getPermissionLevel() >= mMinimumPermissionLevel) {
		allowed = true;
	}

	if(txt.empty()) {
		allowed = false;
	}

	return allowed;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Get::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}

	SyntaxData d;
	d.success = false;
	d.numberToGet = 0;
	d.itemNumber = 0;
	d.containerNumber = 0;

	test_rule1(txt, d);
	
	if(d.success) {
		grammar1(player, d);
		player->Prompt();
		return true;
	}

	test_rule2a(txt, d);
	if(d.success) {
		grammar2a(player, d);
		player->Prompt();
		return true;
	}

	test_rule2b(txt, d);
	if(d.success) {
		grammar2b(player, d);
		player->Prompt();
		return true;
	}

	test_rule2c(txt, d);
	if(d.success) {
		grammar2c(player, d);
		player->Prompt();
		return true;
	}

	test_rule3(txt, d);
	if(d.success) {
		grammar3(player, d);
		player->Prompt();
		return true;
	}

	test_rule4a(txt, d);
	if(d.success) {
		grammar4a(player, d);
		player->Prompt();
		return true;
	}

	test_rule4b(txt, d);
	if(d.success) {
		grammar4b(player, d);
		player->Prompt();
		return true;
	}

	test_rule5a(txt, d);
	if(d.success) {
		grammar5a(player, d);
		player->Prompt();
		return true;
	}

	test_rule5b(txt, d);
	if(d.success) {
		grammar5b(player, d);
		player->Prompt();
		return true;
	}

	test_rule6(txt, d);
	if(d.success) {
		grammar6(player, d);
		player->Prompt();
		return true;
	}

	return help(player);
}

//
// syntax-checking rules start here
Get::SyntaxData Get::test_rule1(std::string command, SyntaxData &data) {
	std::string::iterator first = command.begin();

	bool r = qi::phrase_parse(first, command.end(),
				(
					qi::lexeme[+qi::alpha]
				),
				' ', // skip parser
				data.itemToFind
	);

	if(r && first == command.end()) {
		data.success = true;
		glob.log.debug("Grammar rule1 matched");
		syntaxDebug(data);
	} else {
		data.success = false;
		data.itemToFind = "";
	}

	return data;
}

Get::SyntaxData Get::test_rule2a(std::string command, SyntaxData &data) {
	std::string::iterator first = command.begin();

	// 'get 20 items' rule
	bool r = qi::phrase_parse(first, command.end(),
					(
						qi::int_
						>> qi::lexeme[+qi::alpha]
					),
					' ', // skip parser
					data.numberToGet,
					data.itemToFind
	);

	if(r && first == command.end()) {
		data.success = true;
		glob.log.debug(boost::format("Rule2a succeeded with %1% of %2%") % data.numberToGet % data.itemToFind);
	} else {
		data.success = false;
		data.numberToGet = 0;
		data.itemToFind = "";
	}

	return data;
}

Get::SyntaxData Get::test_rule2b(std::string command, SyntaxData &data) {
	std::string::iterator first = command.begin();

	// 'get item 2' rule
	bool r = qi::phrase_parse(first, command.end(),
					(
						qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ', // skip parser
					data.itemToFind,
					data.itemNumber
	);

	if(r && first == command.end()) {
		data.success = true;
		glob.log.debug(boost::format("Rule2b succeeded with %1% %2%") % data.itemToFind % data.itemNumber);
	} else {
		data.success = false;
		data.itemToFind = "";
		data.itemNumber = 0;
	}

	return data;
}

Get::SyntaxData Get::test_rule2c(std::string command, SyntaxData &data) {
	std::string::iterator first = command.begin();

	// 'get 20 item 2' rule
	bool r = qi::phrase_parse(first, command.end(),
					(
						qi::int_
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ', // skip parser
					data.numberToGet,
					data.itemToFind,
					data.itemNumber
	);

	if(r && first == command.end()) {
		data.success = true;
		glob.log.debug(boost::format("Rule2c matched with %1% of %2% %3%") % data.numberToGet % data.itemToFind % data.itemNumber);
	} else {
		data.success = false;
		data.numberToGet = 0;
		data.itemToFind = "";
		data.itemNumber = 0;
	}

	return data;
}

Get::SyntaxData Get::test_rule3(std::string command, SyntaxData &data) {
	std::string::iterator first = command.begin();

	// 'get item from container' rule
	bool r = qi::phrase_parse(first, command.end(),
					// begin grammar
					(
						qi::lexeme[+qi::alpha]
						>> "from"
						>> qi::lexeme[+qi::alpha]
					),
					' ', // skip parser
					data.itemToFind,
					data.containerToLookIn
	);

	if(r && first == command.end()) {
		data.success = true;
		glob.log.debug(boost::format("Rule3 matched %1% in %2%") % data.itemToFind % data.containerToLookIn);
	} else {
		data.success = false;
		data.itemToFind = "";
		data.containerToLookIn = "";
	}

	return data;
}

Get::SyntaxData Get::test_rule4a(std::string command, SyntaxData &data) {
	std::string::iterator first = command.begin();

	// 'get 20 items from container' rule
	bool r = qi::phrase_parse(first, command.end(),
				(
					qi::int_
					>> qi::lexeme[+qi::alpha]
					>> "from"
					>> qi::lexeme[+qi::alpha]
				),
				' ', // skip parser
				data.numberToGet,
				data.itemToFind,
				data.containerToLookIn
	);

	if(r && first == command.end()) {
		data.success = true;
		glob.log.debug(boost::format("Rule4a matched %1% %2% from %3%") % data.numberToGet % data.itemToFind % data.containerToLookIn);
	} else {
		data.success = false;
		data.numberToGet = 0;
		data.itemToFind = "";
		data.containerToLookIn = "";
	}

	return data;
}

Get::SyntaxData Get::test_rule4b(std::string command, SyntaxData &data) {
	std::string::iterator first = command.begin();

	// 'get item from container 2' rule
	bool r = qi::phrase_parse(first, command.end(),
					(
						qi::lexeme[+qi::alpha]
						>> "from"
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ', // skip parser
					data.itemToFind,
					data.containerToLookIn,
					data.containerNumber
	);

	if(r && first == command.end()) {
		data.success = true;
		glob.log.debug(boost::format("Rule4b matched %1% from %2% %3%") % data.itemToFind % data.containerToLookIn % data.containerNumber);
	} else {
		data.success = false;
		data.itemToFind = "";
		data.containerToLookIn = "";
		data.containerNumber = 0;
	}

	return data;
}

Get::SyntaxData Get::test_rule5a(std::string command, SyntaxData &data) {
	std::string::iterator first = command.begin();

	// 'get 20 items from container 2' rule
	bool r = qi::phrase_parse(first, command.end(),
					(
						qi::int_
						>> qi::lexeme[+qi::alpha]
						>> "from"
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ', // skip parser
					data.numberToGet,
					data.itemToFind,
					data.containerToLookIn,
					data.containerNumber
	);

	if(r && first == command.end()) {
		data.success = true;
		glob.log.debug(boost::format("Rule5a matched %1% %2% from %3% %4%") % data.numberToGet % data.itemToFind % data.containerToLookIn % data.containerNumber);
	} else {
		data.success = false;
		data.numberToGet = 0;
		data.itemToFind = "";
		data.containerToLookIn = "";
		data.containerNumber = 0;
	}

	return data;
}

Get::SyntaxData Get::test_rule5b(std::string command, SyntaxData &data) {
	std::string::iterator first = command.begin();

	// 'get item 2 from container 2' rule
	bool r = qi::phrase_parse(first, command.end(),
					(
						qi::lexeme[+qi::alpha]
						>> qi::int_
						>> "from"
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ', // skip parser
					data.itemToFind,
					data.itemNumber,
					data.containerToLookIn,
					data.containerNumber
	);

	if(r && first == command.end()) {
		data.success = true;
		glob.log.debug(boost::format("Rule5b matched %1% %2% from %3% %4%") % data.itemToFind % data.itemNumber % data.containerToLookIn % data.containerNumber);
	} else {
		data.success = false;
		data.itemToFind = "";
		data.itemNumber = 0;
		data.containerToLookIn = "";
		data.containerNumber = 0;
	}

	return data;
}

Get::SyntaxData Get::test_rule6(std::string command, SyntaxData &data) {
	std::string::iterator first = command.begin();

	// 'get 20 items 2 from container 3' rule
	bool r = qi::phrase_parse(first, command.end(),
					(
						qi::int_
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
						>> "from"
						>> qi::lexeme[+qi::alpha]
						>> qi::int_
					),
					' ', // skip parser
					data.numberToGet,
					data.itemToFind,
					data.itemNumber,
					data.containerToLookIn,
					data.containerNumber
	);

	if(r && first == command.end()) {
		data.success = true;
		glob.log.debug(boost::format("Rule6 matched %1% %2% %3% from %4% %5%") % data.numberToGet % data.itemToFind % data.itemNumber % data.containerToLookIn % data.containerNumber);
	} else {
		data.success = false;
		data.numberToGet = 0;
		data.itemToFind = "";
		data.itemNumber = 0;
		data.containerToLookIn = "";
		data.containerNumber = 0;
	}

	return data;
}

//
// grammar execution starts here
/// executes the  <b>get item</b> grammar
/** This function does the work for a <b>get item</b> command
	@param player A shared pointer to the player calling the command
*/
void Get::grammar1(Player::PlayerPointer player, SyntaxData &data) {
	Container::ContainerPointer container = findContainer(player, data);
	
	if(container != player) {
		syntaxDebug(data);
		getAction(player, container, data);
	}
}

/// executes the  <b>get 20 items</b> grammar
/** This function does the work for a <b>get 20 items</b> command
	@param player A shared pointer to the player calling the command
*/
void Get::grammar2a(Player::PlayerPointer player, SyntaxData &data) {
	// 'get 20 items'
	Container::ContainerPointer container = findContainer(player, data);
	
	if(container != player) {
		getAction(player, container, data);
	}
}

/// executes the  <b>get item 2</b> grammar
/** This function does the work for a <b>get item 2</b> command
	@param player A shared pointer to the player calling the command
*/
void Get::grammar2b(Player::PlayerPointer player, SyntaxData &data) {
	// 'get item 2'
	Container::ContainerPointer container = findContainer(player, data);
	
	if(container != player) {
		getAction(player, container, data);
	}
}

/// executes the <b>get 20 item 3</b> grammar
/** This function does the work for a <b>get 20 item 3</b> command. When multiple similar items are present in
	room, a player uses this command to get a certain number of a specific group.
	@param player A shared pointer to the player calling the command
*/
void Get::grammar2c(Player::PlayerPointer player, SyntaxData &data) {
	Container::ContainerPointer container = findContainer(player, data);
	
	if(container != player) {
		getAction(player, container, data);
	}
}

/// executes the  <b>get item from container</b> grammar
/** This function does the work for a <b>get item from container</b> command. Containers are searched for
	first in the player's own inventory, and, IF NOT FOUND, then searches the room for the container.
	\note If a player has a bag, and wants to get an item from a bag within the room, the player must first
	get the bag, and then get the item from the bag.
	@param player A shared pointer to the player calling the command
*/
void Get::grammar3(Player::PlayerPointer player, SyntaxData &data) {
	// 'get item from container'
	Container::ContainerPointer container = findContainer(player, data);
	
	if(container != player) {
		getAction(player, container, data);
	}
}

/// executes the  <b>get 20 items from container</b> grammar
/** This function does the work for a <b>get 20 items from container</b> command
	\see grammar3()
	@param player A shared pointer to the player calling the command
*/
void Get::grammar4a(Player::PlayerPointer player, SyntaxData &data) {
	// 'get 20 items from container' grammar
	Container::ContainerPointer container = findContainer(player, data);
	
	if(container != player) {
		getAction(player, container, data);
	}
}

/// executes the  <b>get item from container 2</b> grammar
/** This function does the work for a <b>get item from container 2</b> command
	\see grammar3()
	@param player A shared pointer to the player calling the command
*/
void Get::grammar4b(Player::PlayerPointer player, SyntaxData &data) {
	// 'get item from container 2' grammar
	Container::ContainerPointer container = findContainer(player, data);
	
	if(container != player) {
		getAction(player, container, data);
	}
}

/// executes the  <b>get 20 items from container 2</b> grammar
/** This function does the work for a <b>get 20 items from container 2</b> command
	\see grammar3()
	@param player A shared pointer to the player calling the command
*/
void Get::grammar5a(Player::PlayerPointer player, SyntaxData &data) {
	// 'get 20 items from container 2' grammar
	Container::ContainerPointer container = findContainer(player, data);
	
	if(container != player) {
		getAction(player, container, data);
	}
}

/// executes the  <b>get item 2 from container 2</b> grammar
/** This function does the work for a <b>get item 2 from container 2</b> command
	\see grammar3()
	@param player A shared pointer to the player calling the command
*/
void Get::grammar5b(Player::PlayerPointer player, SyntaxData &data) {
	// 'get item 2 from container 2' grammar
	Container::ContainerPointer container = findContainer(player, data);
	
	if(container != player) {
		getAction(player, container, data);
	}
}

/// executes the  <b>get 20 items 2 from container 3</b> grammar
/** This function does the work for a <b>get 20 items 2 from container 3</b> command
	\see grammar3()
	@param player A shared pointer to the player calling the command
*/
void Get::grammar6(Player::PlayerPointer player, SyntaxData &data) {
	// 'get 20 items 2 from container 3' grammar
	Container::ContainerPointer container = findContainer(player, data);
	
	if(container != player) {
		getAction(player, container, data);
	}
}

/// assistive function to help get items
/** This function is a helper to make all <b>get</b> logic go through the same code. It decides how best 
	to get the item(s) based on item properties and the get() arguments
	@param player A PlayerPointer
	@param container The container in which to look for the item
	@param data A <b>syntaxData</b> object that contains the parsed grammer from the command args
*/
void Get::getAction(Player::PlayerPointer player, Container::ContainerPointer container, SyntaxData &data) {
	std::vector<Physical::PhysicalPointer> inv;

	inv = container->containerFind(data.itemToFind);
	syntaxDebug(data);

	if(inv.size() == 0) {
		player->Write(boost::format("%!%? We don't need no stinking %1%") % data.itemToFind);
	} else {
		ObjectLocation loc;
		loc.type = PlayerObject;
		loc.zone = "";
		loc.location = player->getName();

		unsigned int idx = static_cast<unsigned int>(data.itemNumber);
		
		glob.log.debug(boost::format("Found a %1% at index %2%") % data.itemToFind % data.itemNumber);

		if(inv.size() > 1) {
			if(idx > inv.size()) {
				player->Write(boost::format("There is no %1% %2% here!") % data.itemToFind % idx);

				std::stringstream s;
				int i = 1;

				// generate and show a list of matching items that _are_ here
				for(std::vector<Physical::PhysicalPointer>::iterator it = inv.begin(); it != inv.end(); ++it, ++i) {
					s << i << ": " << (*it)->getBriefDescription() << END;
				}

				player->Write(s.str());

				return;
			}

			if(idx > 0) {
				// we use a 1-based index
				--idx;
			}
		}
		
		glob.log.debug(boost::format("Adjusted index is %1%, yielding %2%") % idx % (inv[idx])->getName());

		Collection::CollectionPointer collection = boost::dynamic_pointer_cast<Collection>(inv[idx]);

		if(collection) {
			glob.log.debug("Item is a collection");
			unsigned int quantity = static_cast<unsigned int>(data.numberToGet);

			if(quantity <= 0) {
				// assume they want them all
				quantity = collection->getQuantity();
			}

			if(quantity > collection->getQuantity()) {
				player->Write(boost::format("There are only %1% %2% here, not %3%!") % collection->getQuantity() % data.itemToFind % quantity);
			} else if(quantity == collection->getQuantity()) {
				glob.log.debug("Getting all the items");
				if(player->containerAdd(inv[idx])) {
					inv[0]->setLocation(loc);
					container->containerRemove(inv[0]);
					
					if(quantity == 1) {
						player->Write(boost::format("You pick up the %1%") % data.itemToFind);
					} else {
						player->Write(boost::format("You pick up all the %1%") % data.itemToFind);
					}
				} else {
					player->Write(boost::format("You can't carry the %1%") % data.itemToFind);
				}
			} else {
				glob.log.debug("Not getting all the items");
				
				// getting less than the total quantity
				Physical::PhysicalPointer newItem = glob.Factory.create(inv[idx]->getObjectType());
				
				if(newItem) {
					Collection::CollectionPointer collection = boost::dynamic_pointer_cast<Collection>(newItem);
					if(collection) {
						collection->changeQuantity(data.numberToGet);
						if(player->containerAdd(newItem)) {
							newItem->setLocation(loc);
							collection->changeQuantity(-data.numberToGet); // should be negative!
							player->Write(boost::format("You pick up %1% %2%") % data.numberToGet % data.itemToFind);
						} else {
							player->Write(boost::format("You can't carry %1% %2%") % data.numberToGet % data.itemToFind);
						}
					} else {
						glob.log.error("Get::getAction() made a container that isn't a container!");
						return;
					}
				} else {
					glob.log.error(boost::format("Failed to create new item of type %1%") % inv[idx]->getObjectType());
					player->Write("Sorry, the factory workers are on strike");
				}
			}
		} else { // if(collection)
			glob.log.debug("Item is not a collection");
			
			// the object isn't a collection, get it normally
			if(player->containerAdd(inv[idx])) {
				inv[idx]->setLocation(loc);
				container->containerRemove(inv[idx]);
				player->Write(boost::format("You pick up the %1%") % data.itemToFind);
			} else {
				player->Write(boost::format("You can't carry the %1%") % data.itemToFind);
			}
		}
	} // inv.size() == 0 else case
}

/// helper function to find a container
/** This function looks for a matching container when the grammar calls for it. If no container 
	was specified, it looks in the room. If a container <em>is</em> named, then it first looks for
	that container in the player's inventory. If not found, it continues to look in the room's
	inventory.
	@param player A pointer to the player calling the function
	@param data A struct with parsed grammer in it.
	\return The container in which to look, or the PlayerPointer on error
*/
Container::ContainerPointer Get::findContainer(Player::PlayerPointer player, SyntaxData &data) {
	ObjectLocation loc = player->getLocation();

	Zone::ZonePointer zone = glob.zoneDaemon.getZone(loc.zone);
	if(!zone) {
		glob.log.error(boost::format("Get::findContainer(): Player %1%'s zone %2% cannot be found") % player->getName() % loc.zone);
		return player;
	}

	Room::RoomPointer room = zone->getRoom(loc.location);
	if(!room) {
		glob.log.error(boost::format("Get::findContainer(): Player %1%'s room %2% in zone %3% cannot be found") % player->getName() % loc.location % loc.zone);
		return player;
	}

	if(data.containerToLookIn.empty()) {
		// 'get item' and not 'get item from something'
		return room;
	}

	std::vector<Physical::PhysicalPointer> inv;

	// search player's inventory for the container first
	inv = player->containerFind(data.containerToLookIn);

	if(inv.size() > 0) {
		// container found in player's inventory
		unsigned int idx = static_cast<unsigned int>(data.containerNumber);
		
		if(idx > inv.size()) {
			std::stringstream s;
			int i = 1;
			
			s << "There's no " << data.containerToLookIn << " " << idx << "here!" << END;
			
			for(std::vector<Physical::PhysicalPointer>::iterator it = inv.begin(); it != inv.end(); ++it, ++i) {
				s << i << ": " << (*it)->getBriefDescription() << END;
			}
			
			player->Write(s.str());
			
			return player;
		}

		if(idx > 0) {
			// we use a 1-based numbering system
			--idx;
		}
		
		Container::ContainerPointer container = boost::dynamic_pointer_cast<Container>(inv[idx]);

		if(container) {
			return container;
		} else {
			player->Write(boost::format("Your %1% is not a container!") % data.containerToLookIn);
			return player;
		}
	} else {
		// search the room for the container
		inv = room->containerFind(data.containerToLookIn);

		if(inv.size() == 0) {
			player->Write(boost::format("There is no %1% here") % data.containerToLookIn);
			return player;
		} else {
			unsigned int idx = static_cast<unsigned int>(data.containerNumber);
			
			if(idx > 0) {		
				if(idx > inv.size()) {
					std::stringstream s;
					int i = 1;

					s << "There is no " << data.containerToLookIn << " " << idx << " here!" << END;

					for(std::vector<Physical::PhysicalPointer>::iterator it = inv.begin(); it != inv.end(); ++it, ++i) {
						s << i << ": " << (*it)->getBriefDescription() << END;
					}
					
					player->Write(s.str());
					
					return player;
				} else {
					--idx;
				}
			}

			Container::ContainerPointer container = boost::dynamic_pointer_cast<Container>(inv[idx]);

			if(container) {
				return container;
			} else {
				return player;
			}
		}
	}
}

void Get::syntaxDebug(SyntaxData &data) {
	glob.log.debug(boost::format("Looking for a %1%") % data.itemToFind);
	glob.log.debug(boost::format("ItemNumber is %1%") % data.itemNumber);
	glob.log.debug(boost::format("NumberToGet is %1%") % data.numberToGet);
	glob.log.debug(boost::format("Container is %1%") % data.containerToLookIn);
	glob.log.debug(boost::format("ContainerNumber is %1%") % data.containerNumber);
}
