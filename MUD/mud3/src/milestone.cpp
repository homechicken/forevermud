#include <yaml-cpp/yaml.h>

#include "milestone.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** The constructor sets a few default values that will be overrided on load()
*/
Milestone::Milestone() {
	setName("milestone");
	setObjectType(MilestoneObject);
}

/// Destructor
/** Does nothing
*/
Milestone::~Milestone() {
}

/// returns the brief description
/** This function returns a simple description of this object
	\return a brief description
*/
std::string Milestone::getBrief() const {
	return "a milestone";
}

/// returns the verbose description
/** This function returns the more complex verbose description for this
	object.
	\return a verbose description
*/
std::string Milestone::getVerbose() const {
	return "This is a milestone. Try ~b00read~resing it";
}

/// loads data
/** This function accepts a YAML::Node and attemts to recreate this object
	based on the previously saved data.
	@param node A YAML::Node indicating the beginning of the object
	\return \b true if no exceptions were thrown, otherwise \b false
*/
bool Milestone::Load(const YAML::Node &node) {
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Milestone::milestoneLoad(): Received non-map YAML node!");
		return success;
	}

	try {
		physicalLoad(node["Physical"]);
		readableLoad(node["Readable"]);
		success = true;
		glob.log.debug("Milestone::Load: was successful");
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Milestone::milestoneLoad(): No such map key: %1%") % e.what());
	} catch(YAML::Exception &e) {
		glob.log.error(boost::format("Milestone::milestoneLoad(): YAML generic exception: %1%") % e.what());
	} catch(...) {
		glob.log.error("Milestone::milestoneLoad(): caught a fall-through exception");
	}

	return success;
}

/// dummy function to satisfy pure virtual ancestor class
/** This function is a dummy, objects are never saved on their own. You should
	instead call the overloaded Save(YAML::Emitter &out) function.
	\return false always
*/
bool Milestone::Save() {
	glob.log.error("Milestone::Save(): Called this function instead of the YAML version");
	return false;
}

/// dummy load function to satisfy pure virtual ancestor
/** This function should not be called, and will log an error if it is.
	\return false always
*/
bool Milestone::Load() {
	glob.log.error("Milestone::Load(): Called this function instead of the YAML version");
	return false;
}

/// the true save function for this object
/** This function generates a saveable text description of the Milestone object
	\return true all the time
*/
bool Milestone::Save(YAML::Emitter &out) const {
	glob.log.debug("Milestone::Save(YAML::Emitter) starting");
	out << YAML::BeginMap;
	physicalSave(out);
	readableSave(out);
	out << YAML::EndMap;
	glob.log.debug("Milestone::Save(YAML::Emitter) ending");

	return true;
}

/// generates a description of this object
/** This function generates a description of this object based on its number and
	the inherited Physical object's brief Description
	\return a string describing the Milestone
	\see Readable Physical
*/
std::string Milestone::getMilestoneText() const {
	if(getNumberOfPages() != 1) {
		glob.log.warn(boost::format("Milestone::getMilestoneText(): Milestone %1% does not have any associated text") % getName());
		return "No Text";
	}

	return getPage(1);
}

/// sets the text of this Readable
/** This function sets the visible text of this Readable object
	@param text the text to set
	\todo fix the setWritable() values or privatize this function to only be used when loading
*/
void Milestone::setMilestoneText(const std::string &text) {
	removePage(1);
	addPage(text);
	glob.log.debug(boost::format("Milestone::setMilestoneText(): Milestone has %1% pages") % getNumberOfPages());
}
