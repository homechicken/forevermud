#ifndef MUD_COIN_H
#define MUD_COIN_H


#include <boost/shared_ptr.hpp>
#include <yaml-cpp/yaml.h>

#include "mudconfig.h"
#include "item.h"
#include "collection.h"

class Coin : public Physical, public Collection {
public:
	typedef boost::shared_ptr<Coin> CoinPointer;

	Coin();
	virtual ~Coin();

	bool addAlloy(const std::string &metal, const int percent);
	bool removeAlloy(const std::string &metal);

	unsigned int hasAlloy(const std::string &metal) const;

	bool Load();
	bool Load(const YAML::Node &node);

	bool Save();
	bool Save(YAML::Emitter &out) const;

	void coinSave(YAML::Emitter &out) const;
	bool coinLoad(const YAML::Node &node);

private:
	std::map<std::string, int> mAlloys;

	std::string getBrief() const;
	std::string getVerbose() const;
};

#endif // MUD_COIN_H
