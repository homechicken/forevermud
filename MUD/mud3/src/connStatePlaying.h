#ifndef MUD_CONN_STATE_PLAYING_H
#define MUD_CONN_STATE_PLAYING_H

#include "connectionState.h"

class ConnectionState_Playing : public ConnectionState {
public:
	ConnectionState_Playing();
	virtual ~ConnectionState_Playing();

	void process(const std::string &command);

	bool inPlayState() const { return true; }
private:
};

#endif // MUD_CONN_STATE_PLAYING_H
