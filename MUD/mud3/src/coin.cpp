#include "coin.h"
#include "global.h"

extern Global glob;

Coin::Coin() {
	setName("coin");
	addNickname("coins");
	setObjectType(CoinObject);
}

Coin::~Coin() {
}

bool Coin::addAlloy(const std::string &metal, const int percent) {
	bool success = false;

	// check to see if this metal is not present first
	if(hasAlloy(metal) == 0) {
		int totalPercent = 0;

		for(std::map<std::string, int>::const_iterator it = mAlloys.begin(); it != mAlloys.end(); ++it) {
			totalPercent += it->second;
		}

		if((totalPercent + percent) <= 100) {
			if(!(mAlloys.insert(std::make_pair(metal, percent))).second) {
				glob.log.error(boost::format("Coin::addAlloy(): Failed to insert alloy %1%:%2%") % metal % percent);
			}
			success = true;
		} else {
			glob.log.error(boost::format("Coin::addAlloy(): Cannot add alloys to more than 100%!"));
		}
	}

	int highestPercent = 0;
	std::string highestMetal;

	for(std::map<std::string, int>::const_iterator it = mAlloys.begin(); it != mAlloys.end(); ++it) {
		if(it->second > highestPercent) {
			highestPercent = it->second;
			highestMetal = it->first;
		}
	}

	addNickname(highestMetal);


	return success;
}

bool Coin::removeAlloy(const std::string &metal) {
	bool success = false;

	std::map<std::string, int>::iterator it;

	if((it = mAlloys.find(metal)) != mAlloys.end()) {
		mAlloys.erase(it);
		success = true;
	}

	return success;
}

unsigned int Coin::hasAlloy(const std::string &metal) const {
	unsigned int percent = 0;

	std::map<std::string, int>::const_iterator it;

	if((it = mAlloys.find(metal)) != mAlloys.end()) {
		percent = it->second;
	}

	return percent;
}

bool Coin::Load() {
	glob.log.error("Coin::Load: I was called instead of Load(YAML Node)");
	return false;
}

bool Coin::Save() {
	glob.log.error("Coin::Save: I was called instead of Save(YAML Emitter)");
	return false;
}

bool Coin::Save(YAML::Emitter &out) const {
	out << YAML::BeginMap;

	physicalSave(out);

	collectionSave(out);

	coinSave(out);

	out << YAML::EndMap;

	return true;
}

bool Coin::Load(const YAML::Node &node) {
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Coin::Load: Received non-map YAML node!");
		return success;
	}

	try {
		if(!physicalLoad(node["Physical"])) {
			glob.log.error("Coin::Load(): Unable to load Physical Node");
		} else if(!collectionLoad(node["Collection"])) {
			glob.log.error("Coin::Load(): Unable to load Collection node");
		} else if(!coinLoad(node["Coin"])) {
			glob.log.error("Coin::Load(): Unable to load Coin node");
		} else {
			success = true;
		}
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Coin::Load(): No such map key: %1%") % e.what());
	} catch(YAML::Exception &e) {
		glob.log.error(boost::format("Coin::Load(): YAML exception caught: %1%") % e.what());
	} catch(...) {
		glob.log.error("Coin::Load(): caught a fall-through exception");
	}

	return success;
}

void Coin::coinSave(YAML::Emitter &out) const {
	out << YAML::Key << "Coin" << YAML::Value << YAML::BeginMap;

	out << YAML::Key << "alloys" << YAML::Value << YAML::BeginMap;

	if(mAlloys.size() > 0) {
		for(std::map<std::string, int>::const_iterator it = mAlloys.begin(); it != mAlloys.end(); ++it) {
			out << YAML::Key << it->first << YAML::Value << it->second;
		}
	} else {
		out << YAML::Null;
	}

	out << YAML::EndMap; // alloys

	out << YAML::EndMap; // Coin
}

bool Coin::coinLoad(const YAML::Node &node) {
	bool success = false;
	glob.log.debug("Loading Coin node..");

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Coin::coinLoad(): Received non-map YAML node");
		return success;
	}

	try {
		if(!YAML::IsNull(node["alloys"])) {
			const YAML::Node &alloys = node["alloys"];

			for(YAML::Iterator it = alloys.begin(); it != alloys.end(); ++it) {
				std::string metal;
				int percent;

				it.first() >> metal;
				it.second() >> percent;

				if(!addAlloy(metal, percent)) {
					glob.log.error(boost::format("Coin::coinLoad(): Error adding alloy %1%:%2%") % metal % percent);
				}
			}
		}

		success = true;
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Coin::coinLoad: YAML parser exception caught: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Coin::coinLoad: YAML exception caught (no such map key!): %1%") % e.what());
	} catch(...) {
		glob.log.error("Coin::coinLoad: caught a fall-through exception");
	}

	glob.log.debug("Done loading Coin node");

	return success;

}

std::string Coin::getBrief() const {
	std::string mostMetal;
	int highestPercent = 0;

	std::map<std::string, int>::const_iterator it;

	for(it = mAlloys.begin(); it != mAlloys.end(); ++it) {
		if(it->second > highestPercent) {
			highestPercent = it->second;
			mostMetal = it->first;
		}
	}

	return boost::str(boost::format("%1% %2% coin%3%") % getQuantity() % mostMetal % (getQuantity() > 1 ? "s" : ""));
}

std::string Coin::getVerbose() const {
	return getBriefDescription();
}
