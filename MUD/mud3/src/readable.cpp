#include "readable.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** The constructor sets default values that should be overridden when the object
	is loaded.
*/
Readable::Readable() {
	mIsWritable = false;
	mLanguage = "common";
}
/// Destructor
/** Does nothing
*/
Readable::~Readable() {
}

/// adds a page to the object
/** This function adds a new page of text to the object if it is writable
	@param text the text of the new page
	\return true if the object is writable and the page was added
*/
bool Readable::addPage(const std::string &text) {
	bool success = false;

	if(mIsWritable) {
		mPages.push_back(text);
		success = true;
	}

	return success;
}

/// removes a page from the object
/** This function removes a page from the object by number
	@param page the page number to remove
	\note this function does nothing if the page number was too high
*/
void Readable::removePage(unsigned int page) {
	if(page > mPages.size()) {
		return;
	}

	StringVector::iterator it = mPages.begin();

	it += (page - 1);

	mPages.erase(it);
}

/// gets the text of a single page
/** This function returns the text of a page from this object. Page numbers start at 1, not 0. If the
	page request is invalid, it returns an error string.
	@param page The number of the page to get text for
	\return The text of the corresponding page
*/
std::string Readable::getPage(unsigned int page) const {
	if(page > mPages.size()) {
		return "Page request out of range";
	}

	return mPages[page - 1];
}

/// saves the data for this object
/** This function generates a string of text representing the properties of this readable object.
*/
void Readable::readableSave(YAML::Emitter &out) const {
	out << YAML::Key << "Readable" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "language" << YAML::Value << mLanguage;
	out << YAML::Key << "writable" << YAML::Value << mIsWritable;

	out << YAML::Key << "pages" << YAML::Value << YAML::BeginSeq;

	if(mPages.size() > 0) {
		for(StringVector::const_iterator it = mPages.begin(); it != mPages.end(); ++it) {
			out << YAML::Literal << *it;
		}
	} else {
		out << YAML::Null;
	}

	out << YAML::EndSeq;

	out << YAML::EndMap;
}

/// loads the data for this readable object from a text string
/** This function does the heavy lifting for restoring a readable object from a saved state.
	@param node A YAML::Node pointing to previously saved Readable data
	\return Whether the loading was successful or not
*/
bool Readable::readableLoad(const YAML::Node &node) {
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Readable::readableLoad(): Node is not a map!");
		return success;
	}

	try {
		node["language"] >> mLanguage;
		node["writable"] >> mIsWritable;

		if(!YAML::IsNull(node["pages"])) {
			const YAML::Node &ppg = node["pages"];

			if(ppg.GetType() != YAML::CT_SEQUENCE) {
				glob.log.error("Readable::readableLoad(): 'pages' node is not a sequence!");
			} else {
				for(YAML::Iterator it = ppg.begin(); it != ppg.end(); ++it) {
					std::string page;
					*it >> page;
					mPages.push_back(page);
				}
				success = true;
			}
		} else {
			// Pages node is null, meaning there are no pages, it's empty
			success = true;
		}
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Readable::readableLoad(): YAML parser exception caught: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Readable::readableLoad(): YAML exception caught (no such map key!): %1%") % e.what());
	} catch(...) {
		glob.log.error("Readable::readableLoad(): caught a fall-through exception");
	}

	return success;
}

