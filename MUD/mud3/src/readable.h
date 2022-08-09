#ifndef READABLE
#define READABLE

#include "mudconfig.h"
#include <boost/shared_ptr.hpp>
#include <yaml-cpp/yaml.h>

/// a class that describes objects that can be read
/** This class defines behavior for objects that have text that can be read
*/
class Readable {
public:
	typedef boost::shared_ptr<Readable> ReadablePointer;

	Readable();
	virtual ~Readable();

	/// gets the number of pages in this object
	unsigned int getNumberOfPages() const { return mPages.size(); }

	std::string getPage(unsigned int page) const;

	/// true if the object can be written in or on
	bool getIsWritable() const { return mIsWritable; }

	/// sets whether or not the object can be written in or on
	void setWritable(bool isWritable) { mIsWritable = isWritable; }

	bool addPage(const std::string &text);
	void removePage(unsigned int page);

	bool readableLoad(const YAML::Node &node);

	void readableSave(YAML::Emitter &out) const;

	/// what language is this written in?
	std::string getLanguage() const { return mLanguage; }

	/// sets the language the object is written in
	void setLanguage(const std::string &language) { mLanguage = language; }

private:
	StringVector mPages;	///< the number of pages this object has

	std::string mLanguage;	///< the language it is written in
	bool mIsWritable;	///< whether or not this object may be written in or on
};

#endif // READABLE
