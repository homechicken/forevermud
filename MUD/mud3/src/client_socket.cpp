#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <arpa/telnet.h>

#include "client_socket.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Initializes default values. We assume any connection can handle ANSI color.
	If not, they'll get weird characters on their screen until they turn color off.
	The mutex for this socket is also initialized so we don't have write collisions
	between different threads.
*/
ClientSocket::ClientSocket() {
	mFd = -1;
	mColorblind = false;
	mIn_buffer = "";
	mOut_buffer = "";

	if(pthread_mutex_init(&mBusy, NULL) != 0) {
		perror("ClientSocket::ClientSocket(): mutex initialization error");
		exit(MUTEX_ERROR);
	}
}

/// Destructor
/** Resets the file descriptor to a bad value
*/
ClientSocket::~ClientSocket() {
	mFd = -1;
	pthread_mutex_destroy(&mBusy);
}

/// SocketDriver calls this function to read from this connection
/** This function reads input from its connection, which is then parsed
	by the SocketDriver and placed back in this object's in_buffer.
	\return true if the socket can be read from
	\note Due to the nature of C-style sockets, we have to use a char buffer to
			read the data, and because of that we have to assign it a size. It \e is
			possible that this size will be exceeded, but extremely unlikely.
	\note kMaxSocketInputBufferLength is defined in mudconfig.h
*/
bool ClientSocket::read_socket() {
	unsigned char buffer[kMaxSocketInputBufferLength] = {0};

	int bytes_read = 0,
		total_read = 0;

	bool ret = false;

	bytes_read = read(mFd, buffer, kMaxSocketInputBufferLength);

	total_read += bytes_read;

	glob.log.debug(boost::format("Read %1% bytes of data from descriptor %2%") % bytes_read % mFd);

	if(buffer[0] == IAC) {
		if(bytes_read == kMaxSocketInputBufferLength) {
			glob.log.error("Client sent telnet options, but they filled the buffer!");
			return true;
		}
		processTelnetOptions(buffer);
		glob.statEngine.addBytesIn(total_read);
		return true;
	}

	while(bytes_read == kMaxSocketInputBufferLength) {
		glob.log.warn("ClientSocket::read_socket(): reading full buffer size");
		if(this->lock()) {
			mIn_buffer += (char *)buffer;
			this->unlock();
		}

		bytes_read = read(mFd, buffer, kMaxSocketInputBufferLength);
		total_read += bytes_read;
	}

	if(total_read > 0) {
		if(this->lock()) {
			mIn_buffer += (char *)buffer;
			this->unlock();
			ret = true;
		} else {
			// couldn't lock?
			ret = false;
		}
	} else if(total_read == 0) {
		glob.log.warn(boost::format("ClientSocket::read_socket(): read EOF from client %1%") % mFd);
		ret = false;
	} else {
		// this happens sometimes when telnet window is closed
		glob.log.error(boost::format("ClientSocket::read_socket(): Error reading from client %1%") % mFd);
		ret = false;
	}

	glob.statEngine.addBytesIn(total_read);

	return ret;
}

///	Processes and responds to telnet option requests
/** This function looks for recognized telnet options (cf. RFC854, RFC855, and
	http://www.iana.org/assignments/telnet-options) and responds to them. It will also
	generate debug information for unrecognized options
	\note Telnet options are somewhat complicated. Typically options are received as a control sequence
		starting with the IAC (decimal 255) character. Following it may be any of several options, most
		commonly \c WILL \c WONT \c DO or \c DONT followed by the thing to do/not do. We respond to \c WILL
		with \c DONT, and \c DO with \c WONT. Additionally, MCCP is not currently supported (\c COMPRESS is
		85 and \c COMPRESS2 is 86)
*/
void ClientSocket::processTelnetOptions(unsigned char *options) {
	std::stringstream s;
	unsigned char response[3] = {0};
	int len = 0;
	response[len++] = IAC;

	for(size_t i=1; i<strlen((char *)options); ++i) {
		if(options[i] == IAC) {
			switch(options[i+1]) {
			case WILL:
				switch(options[i+2]) {
				case TELOPT_SGA:	// 3
					// client is willing to suppress go-ahead
					response[len++] = DO;
					response[len++] = TELOPT_SGA;
					glob.log.debug("Responding to IAC,WILL,TELOPT_SGA with IAC,DO,TELOPT_SGA");
					break;
				case TELOPT_NAWS:	// 31
				case TELOPT_TSPEED: // 32
				case TELOPT_TTYPE:	// 24
				case TELOPT_NEW_ENVIRON:	// 39
				default:
					response[len++] = DONT;
					response[len++] = options[i+2];
					glob.log.debug(boost::format("Responding DONT to client request for option %1%") % (int)options[i+2]);
				}
				i += 3;
				break; // case WILL
			case DO:
				switch(options[i+2]) {
				case TELOPT_ECHO:
					response[len++] = WONT;
					response[len++] = TELOPT_ECHO;
					glob.log.debug("Responding to IAC,DO,ECHO with IAC,WONT,ECHO");
					break;
				case TELOPT_SGA:
					response[len++] = WILL;
					response[len++] = TELOPT_SGA;
					glob.log.debug("Responding to IAC,DO,TELOPT_SGA");
					break;
				default:
					response[len++] = WONT;
					response[len++] = options[i+2];
					glob.log.debug(boost::format("Responding WONT to client request for option %2%") % (int)options[i+2]);
				}
				i += 3;
				break; // case DO
			case SB:
				s << "Client is attempting to negotiate suboptions: ";
				while(options[i++] != SE) {
					s << i << ":" << (int)options[i] << " ";
				}
				glob.log.debug(s.str());
				s.str("");
				break;
			default:
				glob.log.error(boost::format("ClientSocket(): Client sent unsupported request for option %1%") % (int)options[i+1]);
				break;
			} // options[i+1] (WILL|DO)
		} // options[i] == IAC
	} // for()

	if(len > 1) {
		// we have option responses to send to the client
		if(this->lock()) {
			int written = write(mFd, response, len);

			glob.statEngine.addBytesOut(written);

			this->unlock();
		}
	}
}

/// Writes data to client
/** This function takes the current out_buffer and writes it out to the client in
	predefined chunk sizes. It also updates the StatEngine with the number of bytes
	written out.
	\see StatEngine
	\return true if able to write the output buffer to the client socket
	\note kMaxSocketBufferWriteSize is defined in mudconfig.h
	\note This function only returns false if unable to lock the mutex.
*/
bool ClientSocket::flush() {
	if(mOut_buffer.empty()) {
		return true;
	}

	int write_size = 0;
	int written = 0;

	if(this->lock()) {
		// write data to socket
		for(unsigned int i = 0; i<mOut_buffer.length(); i += written) {
			write_size = ((mOut_buffer.length() - i) < kMaxSocketBufferWriteSize) ? mOut_buffer.length() - i : kMaxSocketBufferWriteSize;

			written = write(mFd, mOut_buffer.substr(i, write_size).c_str(), write_size);

			if(written < 0) {
				glob.log.error(boost::format("ClientSocket::write_socket(): Problem writing to client %1%") % mFd);
				return false;
			}

			// wipe out contents of buffer after writing
			mOut_buffer.clear();
			glob.statEngine.addBytesOut(written);
		}

		this->unlock();
	} else {
		// couldn't lock
		glob.log.error(boost::format("ClientSocket::write_socket(): Failed to lock client %1%'s mutex") % mFd);
		return false;
	}

	return true;
}

/// assigns the socket's input buffer to a string the clears the buffer
/** This function takes the socket's input buffer and assigns it to a string, then
	erases everything in the input buffer.
	@param buffer an empty string reference to assign the buffer text to
*/
void ClientSocket::get_in_buffer(std::string &buffer) {
	buffer = mIn_buffer;
	mIn_buffer.clear();
}

/// Locks this resource so threads don't fight over it
/** A simple function to limit access to the socket one thread at a time
	\return true if able to lock the mutex
*/
bool ClientSocket::lock() {
	bool success;

	if(pthread_mutex_lock(&mBusy) != 0) {
		success = false;
		glob.log.error("ClientSocket::lock(): Could not lock mutex");
	} else {
		success = true;
	}

	return success;
}

/// Unlocks this resource so other threads can use it
/** A simple function to unlock the mutex and allow another thread access to
	this resource
	\return true if able to unlock the mutex
	\note I have never seen this return false
*/
bool ClientSocket::unlock() {
	bool success;

	if(pthread_mutex_unlock(&mBusy) != 0) {
		success = false;
		glob.log.error("ClientSocket::unlock(): Coudl not unlock mutex");
	} else {
		success = true;
	}

	return success;
}

/// Adds text to this connection's out_buffer
/** This function adds text to the socket's output buffer, parsing ANSI color
	if necessary and formatting the text to the client's X-resolution
	@param text The text to write to the client
	@param width the width of the client's screen
*/
void ClientSocket::to_client(const std::string &text, int width) {
	std::string outText = parseColor(text);
	outText = formatWidth(outText, width);

	if(this->lock()) {
		// lock ok
		mOut_buffer += outText;
		this->unlock();
	}
}

/// overloaded version of to_client() that formats a StringVector instead
/** This function is an overloaded version of to_client() that takes a StringVector
	and formats it into columns and sends it to the client.
	@param list the StringVector to format into columns
	@param width the width of the client's screen
*/
void ClientSocket::to_client(const StringVector &list, int width) {
	std::string::size_type longest = 0;

	for(StringVector::const_iterator it = list.begin(); it != list.end(); ++it) {
		unsigned int l = Utility::stringLength(*it);
		if(l > longest) {
			longest = l;
		}
	}

	int columnWidth = longest + FORMATTING_SPACE;
	int columns = width / columnWidth;

	std::stringstream s;

	int i = 1;
	for(StringVector::const_iterator it = list.begin(); it != list.end(); ++it, ++i) {
		s << std::setw(columnWidth) << std::left << *it;
		if(i == columns) {
			s << END;
			i = 0;	// we are immediately incremented to 1, so this is okay!
		}
	}
	if(this->lock()) {
		mOut_buffer += s.str();
		this->unlock();
	}
}

/// Adds text to this connections in_buffer
/** This function is called by the SocketDriver to add incoming text from the
	socket's file descriptor to the client's input buffer.
	@param text The incoming text from the client
*/
void ClientSocket::from_client(const std::string& text) {
	if(this->lock()) {
		mIn_buffer += text;
		this->unlock();
	}
}

/// Parses txt looking for color tokens
/** This function takes a text string with an arbitrary number of color tokens,
	walks through it linearly, and converts it to ANSI color sequences
	(if the client can view color). It will automatically remove any non-valid
	color sequences from the text string.
	@param txt The text that should be parsed
	\return a string with ANSI control characters if the client can see color,
		or a string with all internal color sequences removed.
*/
std::string ClientSocket::parseColor(const std::string &txt) {
	std::stringstream out;

	// if there aren't enough characters to make a full color token, return
	if(txt.length() < 4) {
		return txt;
	}

	std::string::size_type pos;

	for(pos = 0; pos < txt.length(); ++pos) {
		if(txt[pos] != '~') {
			out << txt[pos];
		} else {
			pos += convertToken(&txt[pos], out);
		}
	}
	return out.str();
}

/// Evaluates a single color sequence for output
/** This function evaluates a single color sequence and generates the proper ANSI
	color sequence, then appends it to the outgoing stream.  If the connection is
	colorblind, then the sequence is skipped. If the token is an escaped color
	sequence, the escape character is dropped and the rest of the token is sent to
	the client without being evaluated.
	@param txt the beginning of an internal color sequence
	@param[out] out a reference to the processed stream to be sent to the client
	\return the number of characters the current token uses
*/
std::string::size_type ClientSocket::convertToken(const char *txt, std::stringstream &out) {
	// handle escaped (~~) color sequences
	if(txt[1] == '~') {
		out << txt[1];
		return 1;
	}

	if(mColorblind) {
		return 3;
	}

	char ansiControl[3] = {27, '[', 0 };

	switch(tolower(txt[1])) {
		case '0':
		case 'n':
			// normal text
			out << ansiControl << "0";
			break;
		case 'b':
			// bold/bright text
			out << ansiControl << "1";
			break;
		case 'd':
			// dim text?
			out << ansiControl << "2";
			break;
		case 'u':
			// underscore text?
			out << ansiControl << "4";
			break;
		case 'f':
			// blinking text
			out << ansiControl << "5";
			break;
		case 'r':
			if(tolower(txt[2]) == 'e' ) {
				switch(tolower(txt[3])) {
					case 's':
						// reset sequence found
						out << ansiControl << "0m";
						break;
					case 'v':
						// reverse video sequence
						out << ansiControl << "7m";
						break;
					case 'x':
						// hidden video
						out << ansiControl << "8m";
						break;
					default:
						// invalid sequence
						return 3;
				}
			} else {
				// invalid sequence
				return 3;
			}
		default:
			// invalid sequence
			return 3;
	}

	// characters are:
	//	k = black
	//	r = red
	//	g = green
	//	y = yellow
	//	b = blue
	//	m = magenta
	//	c = cyan
	//	w = white

	// foreground color
	switch(tolower(txt[2])) {
		case 'k':
			out << ";30";
			break;
		case 'r':
			out << ";31";
			break;
		case 'g':
			out << ";32";
			break;
		case 'y':
			out << ";33";
			break;
		case 'b':
			out << ";34";
			break;
		case 'm':
			out << ";35";
			break;
		case 'c':
			out << ";36";
			break;
		case 'w':
			out << ";37";
			break;
		case '0':
			// no foreground change
			break;
		default:
			// invalid sequence
			out << "m";
			return 3;
	}

	// background color
	switch(tolower(txt[3])) {
		case 'k':
			out << ";40m";
			break;
		case 'r':
			out << ";41m";
			break;
		case 'g':
			out << ";42m";
			break;
		case 'y':
			out << ";43m";
			break;
		case 'b':
			out << ";44m";
			break;
		case 'm':
			out << ";45m";
			break;
		case 'c':
			out << ";46m";
			break;
		case 'w':
			out << ";47m";
			break;
		case '0':
		default:
			// invalid sequence
			out << "m";
			return 3;
	}
	return 3;
}

/// formats a string with newlines at most every \c width characters apart
/** This function takes a string that is about to be sent to a socket, and formats it to the width
	the client has configured in its options. It will not break apart words or newlines, and will skip
	over and not count color sequences if included.
	@param text The text to be modified
	@param width The maximum line length the client will take
	\return A newline-delimited string not exceeding clients specified width in printable characters
*/
std::string ClientSocket::formatWidth(const std::string &text, int width) {
	if(text.find(" ") == std::string::npos) {
		// if there are no spaces, return the text
		return text;
	}

	std::stringstream s;

	int count = 0;
	std::string::size_type start = 0;

	for(std::string::size_type idx = 0; idx < text.length(); ++idx, ++count) {
		if(text[idx] == '\n') {
			count = 0;
			continue;
		}
		if(text[idx] == 27) {
			// ignore color sequences
			while(text[idx] != 'm') {
				idx++;
			}
			continue;
		}
		if(count == width) {
			if(text[idx] == '\r') {
				// halfway through our END output, this is okay
				idx += 1; // skip the next newline character
				count = 0;
				continue;
			}
			while(text[idx] != ' ') {
				--idx;
			}
			s << text.substr(start, idx - start) << END;
			start = idx + 1;
			count = 0;
		}
	}
	s << text.substr(start);
	return s.str();
}
