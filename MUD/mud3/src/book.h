#ifndef MUD_BOOK_H
#define MUD_BOOK_H

#include <yaml-cpp/yaml.h>

#include "mudconfig.h"
#include "physical.h"
#include "readable.h"

/// defines what a book is
/** This class designates the properties of a book
	\note there may be a better way to do this later, such as creating
		a generic "prop" item instead of a specific "book" like this.
	\note The verbose title of a book is set dynamically. If you modify it
		offline, your changes will not take effect.
*/
class Book : public Physical, public Readable {
public:
	Book();
	virtual ~Book();

	void setTitle(const std::string &title);

	/// gets the title
	std::string getTitle() const { return mTitle; }

	void setAuthor(const std::string &author);

	/// gets the author
	std::string getAuthor() const { return mAuthor; }

	bool Load();
	bool Load(const YAML::Node &node);
	bool Save();
	bool Save(YAML::Emitter &out) const;

	bool bookLoad(const YAML::Node &node);
	void bookSave(YAML::Emitter &out) const;


private:
	std::string mTitle;
	std::string mAuthor;

	std::string getBrief() const;
	std::string getVerbose() const;
};

#endif // MUD_BOOK_H
