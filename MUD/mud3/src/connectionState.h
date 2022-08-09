#ifndef MUD_CONNECTION_STATE_H
#define MUD_CONNECTION_STATE_H

#include <string>
#include <boost/shared_ptr.hpp>

// forward declaration to circumvent circular dependency
class Player;

class ConnectionState {
public:
	ConnectionState() {};
	virtual ~ConnectionState() {}

	virtual void process(const std::string &command) = 0;

	virtual bool inPlayState() const = 0;

	boost::shared_ptr<Player> mPlayer;

private:
};

#endif // MUD_CONNECTION_STATE_H
