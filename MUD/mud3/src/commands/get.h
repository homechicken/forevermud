#ifndef MUD_GET_H
#define MUD_GET_H

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#include "command.h"
#include "player.h"

/// creates an object
/** This class allows a player with the proper permissions to create a new object
	in the virtual world.
*/
class Get: public Command {
public:
	struct SynDat {
		SynDat() : success(false), numberToGet(0), itemNumber(0), containerNumber(0) {}
		bool success;
		int numberToGet;
		int itemNumber;
		int containerNumber;
		std::string itemToFind;
		std::string containerToLookIn;
	};
	
	typedef struct SynDat SyntaxData;
	
	static Get & Instance();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

protected:
	virtual ~Get();
	
private:

	
		
	Get();
	Get(const Get &);
	Get & operator=(const Get &);

	SyntaxData test_rule1(std::string command, SyntaxData &data);
	SyntaxData test_rule2a(std::string command, SyntaxData &data);
	SyntaxData test_rule2b(std::string command, SyntaxData &data);
	SyntaxData test_rule2c(std::string command, SyntaxData &data);
	SyntaxData test_rule3(std::string command, SyntaxData &data);
	SyntaxData test_rule4a(std::string command, SyntaxData &data);
	SyntaxData test_rule4b(std::string command, SyntaxData &data);
	SyntaxData test_rule5a(std::string command, SyntaxData &data);
	SyntaxData test_rule5b(std::string command, SyntaxData &data);
	SyntaxData test_rule6(std::string command, SyntaxData &data);

	void grammar1(Player::PlayerPointer player, SyntaxData &data);
	void grammar2a(Player::PlayerPointer player, SyntaxData &data);
	void grammar2b(Player::PlayerPointer player, SyntaxData &data);
	void grammar2c(Player::PlayerPointer player, SyntaxData &data);
	void grammar3(Player::PlayerPointer player, SyntaxData &data);
	void grammar4a(Player::PlayerPointer player, SyntaxData &data);
	void grammar4b(Player::PlayerPointer player, SyntaxData &data);
	void grammar5a(Player::PlayerPointer player, SyntaxData &data);
	void grammar5b(Player::PlayerPointer player, SyntaxData &data);
	void grammar6(Player::PlayerPointer player, SyntaxData &data);

	void getAction(Player::PlayerPointer player, Container::ContainerPointer container, SyntaxData &data);
	Container::ContainerPointer findContainer(Player::PlayerPointer player, SyntaxData &data);
	
	void syntaxDebug(SyntaxData &data);
};
#endif // MUD_GET_H
