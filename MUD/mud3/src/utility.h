#ifndef UTILITY_H
#define UTILITY_H

#include "mudconfig.h"

namespace Utility {
	int toInt(const std::string &str);
	int toInt(float f);

	bool toBool(const std::string &b);

	unsigned int toUInt(const std::string &str);
	unsigned int toUInt(const int val);
	
	std::string::size_type intToStdStringSizeT(const int val);

	std::string toUpper(const std::string &str);
	std::string toLower(const std::string &str);
	std::string toProper(const std::string &str);

	bool iCompare(const std::string &a, const std::string &b);

	StringVector stringToVector(const std::string &data, const std::string &delim = ":");
	StringMap stringToMap(const std::string &data, const std::string &delim = ": ", const char comment = '#');

	std::string stringReplace(const std::string &source, const std::string &target, const std::string &text);

	std::string stringGetFirst(const std::string &source, const std::string &delimiter);
	std::string stringGetFirst(const std::string &source, const std::string &delimiter, std::string &remainder);

	std::string stringClean(const std::string &source);

	unsigned int stringLength(const std::string &source);

	std::string getObjectTypeString(ObjectType type);
	ObjectType getObjectTypeFromString(const std::string &str);

	StringVector stripCommentedLines(const std::string &originalFile);

	bool isBadChar(const std::string &s);
}

#endif // UTILITY_H
