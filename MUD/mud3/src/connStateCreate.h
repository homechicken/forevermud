#ifndef MUD_CONN_STATE_CREATE_H
#define MUD_CONN_STATE_CREATE_H

#include "connectionState.h"

class ConnectionState_Create : public ConnectionState {
public:
	typedef enum {
		StatePassword = 0
	} CSCreate_State;

	ConnectionState_Create();
	virtual ~ConnectionState_Create();

	void process(const std::string &command);

	bool inPlayState() const { return false; }

private:
	CSCreate_State mLocalState;
	bool mWaitPassword;
};

#endif // MUD_CONN_STATE_CREATE_H
