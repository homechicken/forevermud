#include <algorithm>
#include <boost/lexical_cast.hpp>

#include "global.h"

extern Global glob;

namespace Utility {

	/// converts a string to an int
	/** This function converts a string to an integer value
		@param str a string to convert
		\return the \c int value of the string, or 0 on error
		\note The value may very well \e be 0, so checking for 0 isn't the best way
			to determine if an error happened. The best thing to do is to have an idea
			of what you \e should expect and check for that.
	*/
	int toInt(const std::string &str) {
		if(str.empty()) {
			glob.log.warn("Utility::toInt() has empty string");
			return 0;
		}
		int i = 0;
		try {
			i = boost::lexical_cast<int>(str);
		}
		catch(boost::bad_lexical_cast &) {
			glob.log.error("Utility::toInt(): Bad lexical_cast");
		}
		return i;
	}

	int toInt(float f) {
		int i = 0;
		try {
			i = boost::lexical_cast<int>(f);
		}
		catch(boost::bad_lexical_cast &) {
			glob.log.error("Utility::toInt(float): bad lexical_cast");
		}
		return i;
	}

	/// converts a string to an unsigned int
	/** This function converts a string to an unsigned integer value
		@param str a string to convert
		\return the \c unsigned \c int value of the string, or 0
		\note The value may very well \e be 0, so checking for 0 isn't the best way
			to determine if an error happened. The best thing to do is to have an idea
			of what you \e should expect and check for that.
	*/
	unsigned int toUInt(const std::string &str) {
		if(str.empty()) {
			glob.log.warn("Utility::toUInt() has empty string");
			return 0;
		}
		unsigned int ui = 0;
		try {
			ui = boost::lexical_cast<unsigned int>(str);
		}
		catch(boost::bad_lexical_cast &) {
			glob.log.error("Utility::toUInt(string): Bad lexical_cast");
		}
		return ui;
	}
	
	/// converts a signed integer to an unsigned one
	/** This function converts a signed integer to one that is unsigned.
		@param val a signed integer
		\return an unsigned integer of equal value or 0 if negative
	*/
	unsigned int toUInt(const int val) {
		unsigned int ui = 0;
		
		if(val < 0) {
			return ui;
		}
		
		try {
			ui = boost::lexical_cast<unsigned int>(val);
		} catch(boost::bad_lexical_cast &) {
			glob.log.error("Utility::toUInt(int): Bad lexical cast");
		}
		
		return ui;
	}
	
	/// converts an int to a std::string::size_type
	/** This function converts a signed integer to a std::string::size_type
		@param val a signed integer
		\return a std::string::size_type equivalent or 0 on error
	
	*/
	std::string::size_type intToStdStringSizeT(const int val) {
		std::string::size_type t = 0;

		try {
			t = boost::lexical_cast<std::string::size_type>(val);
		} catch(boost::bad_lexical_cast &) {
			glob.log.error("Utility::intToStdStringSizeT(): Bad lexical cast");
		}
		
		return t;
	}

	/// converts a string to upper case
	/** This function takes a string and returns a copy of it all in upper case. I never
		use it, but it's here in case (hehe) you need it
		@param str the string to convert
		\return a copy of the string in all upper-case letters
	*/
	std::string toUpper(const std::string &str) {
		if(str.empty()) {
			glob.log.warn("Utility::toUpper() received empty string");
			return str;
		}
		std::string s(str);
		std::transform(s.begin(), s.end(), s.begin(), toupper);
		return s;
	}

	/// converts a string to lower case
	/** This function takes a string and converts it to all lower-case letters. I use it
		in my insensitive-string-comparison function in this class
		@param str the string to convert
		\return a copy of the string in all lower-case letters
	*/
	std::string toLower(const std::string &str) {
		if(str.empty()) {
			glob.log.warn("Utility::toLower() received an empty string");
			return str;
		}

		std::string s(str);
		std::transform(s.begin(), s.end(), s.begin(), tolower);

		return s;
	}

	/// proper-cases the string
	/** This function reformats a string to all lower-case letters, then upper-cases
		the first letter
		@param str the string to convert
		\return the string in proper case format
	*/
	std::string toProper(const std::string &str) {
		if(str.empty()) {
			glob.log.warn("Utility::toProper() received an empty string");
			return str;
		}

		std::string s = toLower(str);
		s[0] = std::toupper(s[0]);

		return s;
	}

	/// case-insensitive string comparison function
	/** This function compares two strings for equality. It is case-insensitive.
		@param a the first string
		@param b the second string
		\return true if the strings are equal.
	*/
	bool iCompare(const std::string &a, const std::string &b) {
		if(toLower(a) == toLower(b)) {
			return true;
		} else {
			return false;
		}
	}

	/// converts a string to a boolean value
	/** This function takes a string and converts it to a boolean value
		@param b the string to convert
		\return a bool representation of the string, or false
		\note This function returns false even if the string could not be parsed!
	*/
	bool toBool(const std::string &b) {
		if(b.empty()) {
			glob.log.warn("Utility::toBool() has empty string");
			return false;
		}
		if(iCompare(b, "true") || b == "1") {
			return true;
		} else {
			return false;
		}
	}

	/// converts a string into a vector of strings
	/** This function converts a string into a vector of strings based on the delimiter.
		The delimiter will typically be a blank space character, a comma, or some other separator.
		@param data the data to parse
		@param delim the delimiter that separates values in the string
		\return a vector of strings that were separated by the delimiter
	*/
	StringVector stringToVector(const std::string &data, const std::string &delim) {
		StringVector myVector;

		std::string::size_type pos = 0, idx = 0;

		while((idx = data.find(delim, pos)) != std::string::npos) {
			std::string token = data.substr(pos, idx-pos);
			if(!token.empty()) {
				myVector.push_back(token);
			}
			// skip over the delimiter
			pos = idx + delim.length();
		}
		std::string token = data.substr(pos);
		if(!token.empty()) {
			myVector.push_back(token);
		}

		return myVector;
	}

	/// converts a string to a map of string-to-string values
	/** This function converts a string of newline-delimited data to a std::map of
		string-to-string values based on the supplied delimiter. The delimiter will
		typically be a colon.
		@param data the data string to parse
		@param delim the delimiter that separates each string-to-string pair
		@param comment a comment character that denotes the line should be ignored
		\return a std::map of string-to-string values based on the data and parameters sent
	*/
	StringMap stringToMap(const std::string &data, const std::string &delim, const char comment) {
		StringMap myMap;
		const int dLen = delim.length();

		std::string::size_type sPos = 0, sIdx = 0, dPos = 0;

		while((sIdx = data.find('\n', sPos)) != std::string::npos) {
			std::string line = data.substr(sPos, sIdx-sPos);
			if(line.length() > 0 && line.at(0) != comment) {
				dPos = line.find(delim);
				if(dPos != std::string::npos) {
					myMap.insert(std::make_pair(line.substr(0, dPos), line.substr(dPos + dLen)));
				} else {
					// this is where we ignore any line that doesn't have the delimiter in it
				}
			}
			sPos = sIdx + 1;
		}
		// don't forget to add last line!
		std::string line = data.substr(sPos);
		if(line.length() > 0 && line.at(0) != comment) {
			dPos = line.find(delim);
			if(dPos != std::string::npos) {
				if(!myMap.insert(std::make_pair(line.substr(0, dPos), line.substr(dPos + dLen))).second) {
					glob.log.warn(boost::format("Utility::stringToMap(): Cannot add pair %1%:%2% because of duplicate key %1%") % line.substr(0, dPos) % line.substr(dPos+dLen));
				}
			} else {
				glob.log.error("stringToMap: last line not recognized!");
			}
		}

		return myMap;
	}

	/// converts an ObjectType enum to a string value
	/** This function converts an ObjectType enum (see mudconfig.h) into a string
		@param type the ObjectType to convert
		\return the string value of the ObjectType, or an empty string on failure.
	*/
	std::string getObjectTypeString(ObjectType type) {
		// ObjectType is defined in mudconfig.h
		std::string s;
		switch(type) {
			case PlayerObject: s = "PlayerObject"; break;
			case DataObject: s = "DataObject"; break;
			case RoomObject: s = "RoomObject"; break;
			case MilestoneObject: s = "MilestoneObject"; break;
			case ZoneObject: s = "ZoneObject"; break;
			case BookObject: s = "BookObject"; break;
			case CoinObject: s = "CoinObject"; break;
			default: s = "UndefinedObject";
		}
		return s;
	}

	ObjectType getObjectTypeFromString(const std::string &str) {
		ObjectType t = UndefinedObject;

		if(str == "PlayerObject") {
			t = PlayerObject;
		} else if(str == "DataObject") {
			t = DataObject;
		} else if(str == "RoomObject") {
			t = RoomObject;
		} else if(str == "MilestoneObject") {
			t = MilestoneObject;
		} else if(str == "ZoneObject") {
			t = ZoneObject;
		} else if(str == "BookObject") {
			t = BookObject;
		} else if(str == "CoinObject") {
			t = CoinObject;
		}

		return t;
	}


	/// replaces all instances of a string with another string
	/** This function replaces text within a string with something else, returning a blank
		string if either the original string or the target to replace is blank.
		@param source the original string needing replacement
		@param target the string of text to be replaced
		@param text the text you want to repace \c target with
		\return a string with all instances of \c target replaced with \c text
		\note The final argument, \c text, can be blank. This happens if we want to \e remove
			text (or newlines) from a string.
	*/
	std::string stringReplace(const std::string &source, const std::string &target, const std::string &text) {
		std::string result = source;
		if(source.empty() || target.empty()) {
			return result;
		}

		std::string::size_type pos = 0, idx = 0;

		while((pos = result.find(target, idx)) != std::string::npos) {
			result.replace(pos, target.length(), text);
			// skip forward to prevent searching inside \c txt that we just replaced!
			idx = pos + text.length();
		}

		return result;
	}

	/// gets the first part of the string
	/** This searches a string for the first occurrance of the delimiter and returns
		the contents of the string up to that delimiter.
		@param source the original string to search
		@param delimiter the delimiting string that separates what you're looking for
		\return the first part of the string, up to the delimiter
	*/
	std::string stringGetFirst(const std::string &source, const std::string &delimiter) {
		std::string::size_type idx = source.find_first_of(delimiter);
		return source.substr(0, idx);
	}

	/// overloaded version of stringGetFirst that also returns the remainder of the string
	/** This function behaves exactly like stringGetFirst(), except that as a third argument
		it takes an additional string reference that is populated with the remainder of
		the string \e after the first delimiter.
	*/
	std::string stringGetFirst(const std::string &source, const std::string &delimiter, std::string &remainder) {
		std::string::size_type idx = source.find_first_of(delimiter);
		if(idx != std::string::npos) {
			remainder = source.substr(idx + delimiter.length());
		}
		return source.substr(0, idx);
	}

	/// function to strip leading and trailing spaces from a string
	/** This function strips off leading and trailing spaces from a string
		@param source the string to strip down
		\return a new string without leading or trailing spaces
	*/
	std::string stringClean(const std::string &source) {
		std::string temp = source;

		// strip off leading spaces
		std::string::size_type idx = temp.find_first_of(" ");

		while(idx == 0) {
			temp = temp.substr(1);
			idx = temp.find_first_of(" ");
		}

		// strip off trailing spaces
		idx = temp.find_last_of(" ");
		while(idx == temp.length() - 1) {
			temp = temp.substr(0, temp.length() - 1);
			idx = temp.find_last_of(" ");
		}

		return temp;
	}

	/// gets the length of a string, minus color sequences and ANSI control characters
	/** This function calculates the length of a string, disregarding any color sequences
		or ANSI control characters.
		@param source the source to calculate the length of
		\return the number of printable characters in the string
	*/
	unsigned int stringLength(const std::string &source) {
		unsigned int len = 0;
		for(std::string::size_type idx = 0; idx < source.length(); ++idx, ++len) {
			if(source[idx] == 27) {
				// ANSI color sequence found!
				while(source[idx] != 'm') {
					++idx;
				}
				// we need one extra so we don't count the escape (ASCII 27) character
				idx += 1;
				continue;
			}
			if(source[idx] == '~') {
				idx += 3;
				continue;
			}
		}
		return len;
	}

	bool isBadChar(const std::string &s) {
		if(s.empty()) {
			return true;
		}

		if(	s[0] == '#'	||
			s[0] == '\n' ||
			s[0] == '\r')
		{
			return true;
		}

		return false;
	}

	/// removes comments from a newline-delimimted file that has been read in to a string.
	/** This function takes a file's data in newline-delimited text and strips out each line that
		begins with the comment character '#'. It will also remove blank lines.
		@param originalFile The contents of the file
		\return a StringVector with commented-out lines removed
	*/
	StringVector stripCommentedLines(const std::string &originalFile) {
		StringVector lines = stringToVector(originalFile, "\n");

		for(StringVector::iterator it = lines.begin(); it != lines.end(); ++it) {
			lines.erase(std::remove_if(lines.begin(), lines.end(), isBadChar), lines.end());
		}

		return lines;
	}

}
