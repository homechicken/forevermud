#ifndef EBSOTG_H
#define EBSOTG_H

#include "eventCommand.h"

/// removes the bsotg condition when the time expires
/** This class runs the even that happens when an admin bsotg expires on a player
*/
class eBsotg : public EventCommand {
public:
	eBsotg();
	virtual ~eBsotg();

	void process(const std::string &target, ObjectType type, const std::vector<std::string> &arguments);
private:

};

#endif // EBSOTG_H
