#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

/// a class that describes a message to be sent
/** This class defines what a message is, including the type, destination, header,
	who it's from, etc. It's similar to an SMTP message but without the spam.
	\see MessageDaemon for delivery method
*/
class Message {
public:
	/// defines the different types of messages sent to containers
	typedef enum {
		Chat = 0,
		Login,
		Quit,
		Speech,
		Emote,
		Social,
		Tell,
		Information,
		Mail,
		EntranceExit
	} MessageType;

	/// typedef to make declaring shared pointers easier
	typedef boost::shared_ptr<Message> MessagePointer;

	Message();
	~Message();

	/// set the type of the Message
	void setType(MessageType type) { mType = type; }
	/// get the type of the Message
	MessageType getType() const { return mType; }

	/// set the Message header
	void setHeader(const std::string &header) { mHeader = header; }
	/// get the Message header
	std::string getHeader() const { return mHeader; }

	/// set who this Message is from
	void setFrom(const std::string &from) { mFrom = from; }
	/// get who this Message is from
	std::string getFrom() const { return mFrom; }

	/// set to whom this Message should be delivered
	void setRcpt(const std::string &rcpt) { mRcpt = rcpt; }
	///get the recipient of this Message
	std::string getRcpt() const { return mRcpt; }

	/// set the body of this Message
	void setBody(const std::string &body) { mBody = body; }
	/// overloaded to allow setting message with a boost::format string
	void setBody(const boost::format &body) { mBody = boost::str(body); }
	/// gets the body of this Message
	std::string getBody() const { return mBody; }

private:
	MessageType mType;		///< the type of message this Message is
	std::string mHeader;	///< the Message header, here if we need to pass extra information (such as channel name)
	std::string mFrom;		///< the Message sender
	std::string mRcpt;		///< the Message recipient
	std::string mBody;		///< the body of this Message
};

#endif // MESSAGE_H
