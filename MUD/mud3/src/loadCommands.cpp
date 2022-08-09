#include "commandHandler.h"

#include "say.h"
#include "quit.h"
#include "uptime.h"
#include "when.h"
#include "stats.h"
#include "who.h"
#include "idle.h"
#include "help.h"
#include "ban.h"
#include "config.h"
#include "save.h"
#include "channel.h"
#include "tell.h"
#include "emote.h"
#include "bsotg.h"
#include "social.h"
#include "look.h"
#include "read.h"
#include "alias.h"
#include "test.h"
#include "description.h"
#include "status.h"
#include "map.h"
#include "create.h"
#include "get.h"
#include "shutdown.h"

/// loads all commands into the CommandHandler
/** This function sets up the list of commands that the CommandHandler can call.
	Expect it to get pretty long.
*/
void CommandHandler::loadCommands(void) {
	mCommandList["say"] = &Say::Instance();
	mCommandList["quit"] = &Quit::Instance();
	mCommandList["uptime"] = &Uptime::Instance();
	mCommandList["when"] = &When::Instance();
	mCommandList["stats"] = &Stats::Instance();
	mCommandList["who"] = &Who::Instance();
	mCommandList["idle"] = &Idle::Instance();
	mCommandList["help"] = &Help::Instance();
	mCommandList["ban"] = &Ban::Instance();
	mCommandList["config"] = &Config::Instance();
	mCommandList["save"] = &Save::Instance();
	mCommandList["channel"] = &Channel::Instance();
	mCommandList["tell"] = &Tell::Instance();
	mCommandList["emote"] = &Emote::Instance();
	mCommandList["bsotg"] = &Bsotg::Instance();
	mCommandList["social"] = &Social::Instance();
	mCommandList["look"] = &Look::Instance();
	mCommandList["read"] = &Read::Instance();
	mCommandList["alias"] = &Alias::Instance();
	mCommandList["test"] = &Test::Instance();
	mCommandList["description"] = &Description::Instance();
	mCommandList["status"] = &Status::Instance();
	mCommandList["map"] = &Map::Instance();
	mCommandList["create"] = &Create::Instance();
	mCommandList["get"] = &Get::Instance();
	mCommandList["shutdown"] = &Shutdown::Instance();

}
