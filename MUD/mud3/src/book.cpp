#include "book.h"

#include "global.h"
extern Global glob;

#include "utility.h"

/// the constructor
/** This constructor sets a few default values for a typical book
*/
Book::Book() {
	setName("book");
	setWritable(true);
	setObjectType(BookObject);
}

/// the destructor
/** does nothing
*/
Book::~Book() {
}

/// sets the title
/** This function sets the title of the book
	@param title the title of the book
*/
void Book::setTitle(const std::string &title) {
	mTitle = title;
}

/// sets the author of the book
/** This funtion sets the author of the book
	\see setTitle()
	@param author the author of the book
*/
void Book::setAuthor(const std::string &author) {
	mAuthor = author;
}

/// returns the brief description
/** This function returns a brief description of the book
*/
std::string Book::getBrief() const {
	return "a book";
}

/// returns the verbose description
/** This function returns the verbose description of the book depending
	on the title and author information available.
*/
std::string Book::getVerbose() const {
	std::string str;

	if(mTitle.empty() && mAuthor.empty()) {
		str = "It looks like an average, everyday book";
	} else {
		if(mTitle.empty()) {
			str = "The title is faded, but you can just read the author's name: " + mAuthor;
		} else if(mAuthor.empty()) {
			str = mTitle;
		} else {
			str = mTitle + " by " + mAuthor;
		}
	}

	return str;
}

/// a fake load function
/** This function is here to satisfy the virtual requirement of a parent class
	\return always false
*/
bool Book::Load() {
	glob.log.error("Book::Load: I was called instead of Load(YAML Node)");
	return false;
}

/// a fake save function
/** This function is here to satisfy the virtual requirement of a parent class
	\return always false
*/
bool Book::Save() {
	glob.log.error("Book::Save: I was called instead of Save(YAML Emitter)");
	return false;
}

/// the real load function
/** This is the real load function for a book. It should be passed a YAML node
	that is currently in a MAP state
	@param node A YAML::Node in the map state
	\return true if able to load correctly
*/
bool Book::Load(const YAML::Node &node) {
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Book::Load: Received non-map YAML node!");
		return success;
	}

	try {
		if(!physicalLoad(node["Physical"])) {
			glob.log.error("Book::Load(): Unable to load Physical Node");
		} else if(!readableLoad(node["Readable"])) {
			glob.log.error("Book::Load(): Unable to load Readable node");
		} else if(!bookLoad(node["Book"])) {
			glob.log.error("Book::Load(): Unable to load Book node");
		} else {
			success = true;
		}
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Book::Load(): No such map key: %1%") % e.what());
	} catch(YAML::Exception &e) {
		glob.log.error(boost::format("Book::Load(): YAML exception caught: %1%") % e.what());
	} catch(...) {
		glob.log.error("Book::Load(): caught a fall-through exception");
	}

	return success;
}

/// the real save function
/** This is the true save function for the Book class. It should be passed a
	YAML Emitter in the Sequence state (because that's where all objects save
	their contents) and it will write out its own data.
	@param out a YAML Emitter
	\return always true
*/
bool Book::Save(YAML::Emitter &out) const {
	out << YAML::BeginMap;

	physicalSave(out);

	readableSave(out);

	bookSave(out);

	out << YAML::EndMap;

	return true;
}

/// saves book-specific information
/** This function writes out properties specific to a book
	@param out a YAML Emitter
*/
void Book::bookSave(YAML::Emitter &out) const {
	out << YAML::Key << "Book" << YAML::Value << YAML::BeginMap;

	out << YAML::Key << "title" << YAML::Value ;
	if(mTitle.empty()) {
		out << YAML::Null;
	} else {
		out << mTitle;
	}

	out << YAML::Key << "author" << YAML::Value;
	if(mAuthor.empty()) {
		out << YAML::Null;
	} else {
		out << mAuthor;
	}

	out << YAML::EndMap;
}

/// loads book-specific information
/** This function loads properties previously written out for the book
	@param node a YAML Node pointing to the Book section of the save file
*/
bool Book::bookLoad(const YAML::Node &node) {
	bool success = false;

	try {
		if(!YAML::IsNull(node["title"])) {
			node["title"] >> mTitle;
		}

		if(!YAML::IsNull(node["author"])) {
			node["author"] >> mAuthor;
		}

		success = true;
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Book::bookLoad(): No such map key: %1%") % e.what());
	} catch(YAML::Exception &e) {
		glob.log.error(boost::format("Book::bookLoad(): YAML generic exception: %1%") % e.what());
	} catch(...) {
		glob.log.error("Book::bookLoad(): caught a fall-through exception");
	}

	return success;
}
