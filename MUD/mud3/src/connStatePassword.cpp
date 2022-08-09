#include "connStatePassword.h"
#include "global.h"
#include "zone.h"
#include "room.h"
#include "message.h"
#include "utility.h"

extern Global glob;

ConnectionState_Password::ConnectionState_Password() {
	glob.log.debug("Created a password state handler");
	mNumberOfAttempts = 0;
}

ConnectionState_Password::~ConnectionState_Password() {
	glob.log.debug("Destroyed a password state handler");
}

void ConnectionState_Password::process(const std::string &command) {
	if(!mPlayer) {
		glob.log.error("Password State has no valid PlayerPointer");
		return;
	}

	if(command != mPlayer->getPassword()) {
		mNumberOfAttempts++;

		if(mNumberOfAttempts >= MAX_LOGIN_ATTEMPTS) {
			glob.log.error(boost::format("Player %1% from %2% failed 3 login attempts") % mPlayer->getName() % mPlayer->getHostIP());
			glob.playerDatabase.remove(mPlayer);
		} else {
			mPlayer->Write("Incorrect password. You may try again: ");
		}
	} else {
		// notify everyone of a the new arrival
		Message::MessagePointer message = Message::MessagePointer(new Message);
		message->setType(Message::Login);
		message->setFrom(mPlayer->getName());
		message->setBody(boost::format("%1% has entered the world") % Utility::toProper(mPlayer->getName()));
		glob.playerDatabase.broadcast(message);

		// add the player to the room
		ObjectLocation loc = mPlayer->getLocation();
		Room::RoomPointer room;
		Zone::ZonePointer zone;

		std::string startRoom, startZone;
		startRoom = glob.Config.getStringValue("StartingRoom");
		startZone = glob.Config.getStringValue("StartingZone");

		if(startRoom.empty() || startZone.empty()) {
			glob.log.error("ConnStatePassword: No Starting Room or Zone configured. This is very, very bad.");
		}

		switch(loc.type) {
		case RoomObject:
			zone = glob.zoneDaemon.getZone(loc.zone);

			if(!zone) {
				glob.log.error(boost::format("ConnStatePassword: Zone %1% (player %2%) does not exist! Moving to starting room") % loc.zone % mPlayer->getName());
				zone = glob.zoneDaemon.getZone(startZone);
				room = zone->getRoom(startRoom);
				loc.zone = startZone;
				loc.location = startRoom;
				mPlayer->setLocation(loc);
			}

			room = zone->getRoom(loc.location);

			if(!room) {
				glob.log.warn(boost::format("ConnStatePassword: Room %1% does not exist in zone %2%") % loc.location % loc.zone);

				zone = glob.zoneDaemon.getZone(startZone);
				room = zone->getRoom(startRoom);

				if(!room) {
					glob.log.error(boost::format("ConnStatePassword: The default room %1% in zone %2% is not valid!") % startRoom % startZone);
					assert(0);
				}

				loc.zone = startZone;
				loc.location = startRoom;
				mPlayer->setLocation(loc);
				glob.log.warn(boost::format("ConnStatePassword: Setting player location to zone %1% room %2%") % loc.zone % loc.location);
			}

			if(!room->containerAdd(mPlayer)) {
				glob.log.warn(boost::format("ConnStatePassword: Could not add player %1% object to room %2% in zone %3%") % mPlayer->getName() % loc.location % loc.zone);
				glob.playerDatabase.remove(mPlayer);
			}
			break;
		default:
			glob.log.error("ConnStatePassword: Player is not in a RoomObject");
			mPlayer->Write("Cannot restore your location!");
			zone = glob.zoneDaemon.getZone(startZone);
			room = zone->getRoom(startRoom);
			loc.zone = startZone;
			loc.location = startRoom;
			mPlayer->setLocation(loc);
			if(!room->containerAdd(mPlayer)) {
				glob.log.error("ConnStatePassword: Default case, can't add player to starting room!");
				glob.playerDatabase.remove(mPlayer);
			}
			break;
		}

		mPlayer->Write(room->getFullDescription(mPlayer->getName()));
		mPlayer->Prompt();

		// it's important to do this last, otherwise we lose the mPlayer pointer for this object!
		mPlayer->setConnectionState(Connection::ConnState_Play);
	}
}
