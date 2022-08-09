#ifndef MILESTONE
#define MILESTONE

#include <yaml-cpp/yaml.h>

#include "mudconfig.h"
#include "physical.h"
#include "readable.h"

/// a special class to celebrate milestones in MUD3
/** This class was designed to celebrate milestone achievements in MUD3
*/
class Milestone : public Physical, public Readable {
public:
	Milestone();
	virtual ~Milestone();

	bool Load();
	bool Load(const YAML::Node &node);
	bool Save();
	bool Save(YAML::Emitter &out) const;

	std::string getMilestoneText() const;
	void setMilestoneText(const std::string &text);

private:
	std::string getBrief() const;
	std::string getVerbose() const;
};

#endif // MILESTONE
