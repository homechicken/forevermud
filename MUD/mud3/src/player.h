#ifndef MUD_PLAYER_H
#define MUD_PLAYER_H

#include <boost/shared_ptr.hpp>

#include "mudconfig.h"
#include "connection.h"
#include "sentient.h"
#include "container.h"
#include "messageDaemon.h"

/// a class describing what a player is and does
/** This class has all the information you need for a player
*/
class Player : public Connection, public Sentient, public Container {
public:
	typedef boost::shared_ptr<Player> PlayerPointer;

	/// defines the different permission levels
	typedef enum {
		PlayerPermissions = 0,
		AdminPermissions
	} PermissionLevel;

	Player();
	~Player();

	/// sets the player's password
	void setPassword(const std::string &password) { mPassword = password; }
	/// gets the player's password
	std::string getPassword() const { return mPassword; }

	void processMessage(Message::MessagePointer message);

	bool Save();
	bool Save(YAML::Emitter &out) const;
	bool Load();
	bool Load(const YAML::Node &node);

	/// gets the PermissionLevel this player has
	PermissionLevel getPermissionLevel() const { return mPermissionLevel; }

	/// sets the player's PermissionLevel
	void setPermissionLevel(PermissionLevel p) { mPermissionLevel = p; }

	std::string getAlias(const std::string &alias) const;
	StringVector getAliasList() const;

	void addAlias(const std::string &alias, const std::string &command);
	void removeAlias(const std::string &alias);

	void Prompt();

	/// sets the player's custom command prompt
	void setPrompt(const std::string &prompt)	{ mPrompt = prompt; }
	/// returns the player's prompt
	std::string getPrompt() const { return mPrompt; }

	/// increments the login attempt counter
	void addLoginAttempt()						{ mLoginAttempts++; }
	/// returns the number of attempted (and failed) logins
	int getLoginAttempts() const { return mLoginAttempts; }

	void heartbeat();

	bool playerLoad(const YAML::Node &node);
	void playerSave(YAML::Emitter &out) const;

	std::string getStatusString() const;

	void changeDescription(const std::string &desc) { mDescription = desc; }

private:
	int mLoginAttempts;	///< Number of attempts to login with incorrect password
	std::string mPassword;	///< Player's password
	std::string mPrompt;	///< The player's custom command prompt

	StringVector mChatChannels;	///< The client's subscribed chat channels

	std::string playerGetSaveString() const;

	PermissionLevel mPermissionLevel; ///< The level at which this player can interact with the server

	StringMap mAliases;	///< a vector of command aliases the client uses

	std::string::size_type convertPromptToken(const char *txt, std::stringstream &out);

	std::string getBrief() const;
	std::string getVerbose() const;

	std::string mDescription;
};

#endif // MUD_PLAYER_H
