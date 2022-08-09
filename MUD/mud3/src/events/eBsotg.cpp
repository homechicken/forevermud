#include "eBsotg.h"
#include "global.h"

extern Global glob;

/// Constructor
/** Does nothing
*/
eBsotg::eBsotg() {
}

/// Destructor
/** Does nothing
*/
eBsotg::~eBsotg() {
}

/// runs commands when the event is triggered
/** This function is what is run when the expire event is triggered.
*/
void eBsotg::process(const std::string &target, ObjectType type, const std::vector<std::string> &arguments) {
	Player::PlayerPointer player;

	switch(type) {
	case RoomObject:
		// do nothing, no reason to punish a room!
		break;
	case PlayerObject: // fall through, default type is player
	default:
		player = glob.playerDatabase.getPlayer(target);
		if(!player) {
			return;
		}
		player->removeCondition("bitchslapped");
		player->Write("Bitchslap of the Gods has worn off");
		player->Prompt();
	}
}
