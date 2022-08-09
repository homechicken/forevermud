#ifndef MUD_CONN_STATE_PASSWORD_H
#define MUD_CONN_STATE_PASSWORD_H

#include "connectionState.h"

class ConnectionState_Password : public ConnectionState {
public:
	ConnectionState_Password();
	virtual ~ConnectionState_Password();

	void process(const std::string &command);

	bool inPlayState() const { return false; }

private:
	unsigned int mNumberOfAttempts;
};

#endif // MUD_CONN_STATE_PASSWORD_H
