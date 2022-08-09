#ifndef MUD_CONN_STATE_CLOSED_H
#define MUD_CONN_STATE_CLOSED_H

#include "connectionState.h"

class ConnectionState_Closed : public ConnectionState {
public:
	ConnectionState_Closed();
	virtual ~ConnectionState_Closed();

	void process(const std::string &command);

	bool inPlayState() const { return false; }
private:

};

#endif // MUD_CONN_STATE_CLOSED_H

