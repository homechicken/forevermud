#include "connStateCreate.h"

#include "utility.h"
#include "global.h"
extern Global glob;

ConnectionState_Create::ConnectionState_Create() {
	mLocalState = StatePassword;
	glob.log.debug("Created a create state handler");
}

ConnectionState_Create::~ConnectionState_Create() {
	glob.log.debug("Destroyed a create state handler");
}

void ConnectionState_Create::process(const std::string &command) {
	bool finished = false;

	switch(mLocalState) {
	case StatePassword:
		mPlayer->setPassword(command);
		finished = true;
		break;
	default: /// \todo more create-related stuff (eg choose race, new player tutorial) will go here
		mLocalState = StatePassword;
		glob.log.error("Create state handler: unrecognized state. Reset to prompt for password");
	}

	if(finished) {
		std::string startRoom, startZone;

		startRoom = glob.Config.getStringValue("StartingRoom");
		startZone = glob.Config.getStringValue("StartingZone");

		if(startRoom.empty() || startZone.empty()) {
			glob.log.error("ConnStateCreate: No Starting Room or Zone Configured! This is really, really bad.");
		}

		ObjectLocation loc;
		loc.location = startRoom;
		loc.zone = startZone;
		loc.type = RoomObject;
		mPlayer->setLocation(loc);

		// add some default aliases
		mPlayer->addAlias("n", "north");
		mPlayer->addAlias("s", "south");
		mPlayer->addAlias("e", "east");
		mPlayer->addAlias("w", "west");
		mPlayer->addAlias("ne", "northeast");
		mPlayer->addAlias("nw", "northwest");
		mPlayer->addAlias("se", "southeast");
		mPlayer->addAlias("sw", "southwest");
		mPlayer->addAlias("u", "up");
		mPlayer->addAlias("d", "down");
		mPlayer->addAlias("chat", "channel chat");
		mPlayer->addAlias("prompt", "config prompt");
		mPlayer->addAlias("l", "look");

		Message::MessagePointer message = Message::MessagePointer(new Message);
		message->setType(Message::Login);
		message->setFrom(mPlayer->getName());
		message->setBody(boost::format("%1% is born into the world") % Utility::toProper(mPlayer->getName()));
		glob.playerDatabase.broadcast(message);

		mPlayer->Save();
		mPlayer->setConnectionState(Connection::ConnState_Play);

		// tell the player a little about their environment
		Zone::ZonePointer zone = glob.zoneDaemon.getZone(loc.zone);
		if(!zone) {
			glob.log.error(boost::format("Look::process(): Player %1%'s zone %2% cannot be found") % mPlayer->getName() % loc.zone);
			mPlayer->Write("Something really bad just happened. Sorry about that.");
			return;
		}

		Room::RoomPointer room = zone->getRoom(loc.location);
		if(!room) {
			glob.log.error(boost::format("Look::process(): Player %1%'s room %2% in zone %3% cannot be found") % mPlayer->getName() % loc.location % loc.zone);
			mPlayer->Write("Something really bad just happened. Sorry about that.");
			return;
		}

		mPlayer->Write(room->getFullDescription(mPlayer->getName()));
		mPlayer->Prompt();
	}
}
