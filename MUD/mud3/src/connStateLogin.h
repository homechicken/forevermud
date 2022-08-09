#ifndef MUD_CONN_STATE_LOGIN_H
#define MUD_CONN_STATE_LOGIN_H

#include "connectionState.h"

class ConnectionState_Login : public ConnectionState {
public:
	ConnectionState_Login();
	virtual ~ConnectionState_Login();

	void process(const std::string &command);

	bool inPlayState() const { return false; }

private:
	bool mWaiting;
	bool mCreate;
};

#endif // MUD_CONN_STATE_LOGIN_H
