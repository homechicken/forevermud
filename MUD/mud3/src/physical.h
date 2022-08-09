#ifndef MUD_PHYSICAL_H
#define MUD_PHYSICAL_H

#include <boost/uuid/uuid.hpp>
#include <yaml-cpp/yaml.h>

#include "mudconfig.h"
#include "message.h"
#include "utility.h"

/// a class to describe the behavior of all physical objects
/** This class describes all physical objects (well, they're \e virtual physical
	objects, anyway) and their behavior.
*/
class Physical {
public:
	typedef boost::shared_ptr<Physical> PhysicalPointer;

	/// defines the different states an item may exist in
	typedef enum {
		Invalid = 0,
		Destruct,
		Solid,
		Liquid,
		Gas,
		Plasma
	} MatterState;

	Physical();
	virtual ~Physical() {};

	/// sets the unique ID for this object
	void setId(const boost::uuids::uuid id) { mId = id; }
	/// gets the unique ID for this object
	boost::uuids::uuid getId()	{ return mId; }

	/// gets how long the object is
	unsigned int getLength() const { return mLength; }
	/// sets how long the object is
	bool setLength(const unsigned int l) { mLength = l; return true; }

	/// gets how wide the object is
	unsigned int getWidth() const { return mWidth; }
	/// sets how wide the object is
	bool setWidth(const unsigned int w) { mWidth = w; return true; }

	/// gets how high the object is
	unsigned int getHeight() const { return mHeight; }
	/// sets how high the object is
	bool setHeight(const unsigned int d) { mHeight = d; return true; }

	/// gets how heavy the object is
	unsigned int getWeight() const { return mWeight; }
	/// sets how heavy the object is
	bool setWeight(const unsigned int w) { mWeight = w; return true; }

	/// gets the current object temperature
	int getTemperature() const { return mTemperature; }
	/// sets the current object temperature
	void setTemperature(const int t) { mTemperature = t; }

	/// gets the object's insulation value
	int getInsulation() const { return mInsulation; }
	/// sets the object's insulation value
	void setInsulation(const int t) { mInsulation = t; }

	/// gets the object's name
	std::string getName() const { return mName; }
	/// sets the object's name
	void setName(const std::string &name) { mName = name; }

	/// gets the object's brief description
	std::string getBriefDescription() const { return getBrief(); }

	/// gets the object's verbose description
	std::string getVerboseDescription() const { return getVerbose(); }

	bool addCondition(const std::string &cond);
	bool hasCondition(const std::string &cond) const;
	bool removeCondition(const std::string &cond);

	StringVector getConditions() const { return mConditions; }

	/// sets what type this object is
	void setObjectType(ObjectType type) { mObjectType = type; }
	/// gets the type of this object
	ObjectType getObjectType() const { return mObjectType; }

	/// pure virtual loading function to be overridden by the inheritor
	virtual bool Load() = 0;
	virtual bool Load(const YAML::Node &node) = 0;
	/// pure virtual saving function to be overridden by the inheritor
	virtual bool Save() = 0;
	virtual bool Save(YAML::Emitter &out) const = 0;

	/// gets the current location
	ObjectLocation getLocation() const { return mObjectLocation; }
	/// sets the current location
	void setLocation(const ObjectLocation &location) { mObjectLocation = location; }

	/// adds a nickname for the object
	void addNickname(const std::string &name);

	/// gets a vector of all nicknames that refer to this object
	StringVector getNicknames() const { return mNicknames; }

	bool areYouA(const std::string &name) const;

	void physicalSave(YAML::Emitter &out) const;
	bool physicalLoad(const YAML::Node &node);

	void equalizeTemperature(const int temp);

	/// returns the physical state of the item
	MatterState getMatterState() const { return mState; }

	/// Returns the temperature at which this item turns to a liquid
	int getSolidToLiquidTemp() const { return mSolidToLiquidTemp; }
	/// Sets the temperature at which this item turns to a liquid
	void setSolidToLiquidTemp(const int t) { mSolidToLiquidTemp = t; }

	/// Returns the temperature at which this item turns to a gas
	int getLiquidToGasTemp() const { return mLiquidToGasTemp; }
	/// Sets the temperature at which this item turns to a gas
	void setLiquidToGasTemp(const int t) { mLiquidToGasTemp = t; }

	/// Returns the temperature at which this item turns to plasma
	int getGasToPlasmaTemp() const { return mGasToPlasmaTemp; }
	/// Sets the temperature at which this item turns to a gas
	void setGasToPlasmaTemp(const int t) { mGasToPlasmaTemp = t; }

	/// Returns true if this item is destroyed when it reaches Liquid temperature
	bool getDestroyedIfNotSolid() const { return mDestroyedIfNotSolid; }
	/// Sets whether this item is destroyed upon PhysicalState change to Liquid
	void setDestroyedIfNotSolid(bool b)		{ mDestroyedIfNotSolid = b; }

	/// Returns the strength of this item's magnetic field
	int getMagneticStrength() const { return mMagneticStrength; }
	/// Sets this item's magnetic strength
	void setMagneticStrength(const int s) { mMagneticStrength = s; }

	std::string getMatterStateString() const;
	std::string getMatterStateString(MatterState state) const;

private:
	boost::uuids::uuid mId;	///< unique identifier for this object

	unsigned int mLength;	///< how long
	unsigned int mWidth;	///< how wide
	unsigned int mHeight;	///< how high
	unsigned int mWeight;	///< how heavy

	int mTemperature;	///< the object's temperature
	int mInsulation;	///< how insulated is the object

	std::string mName;	///< the object's \e internal name

	virtual std::string getBrief() const = 0;
	virtual std::string getVerbose() const = 0;

	StringVector mConditions;	///< a vector of conditions that currently affect this object
	StringVector mNicknames;	///< a vector of nicknames that this object is known by

	ObjectType mObjectType;	///< the type of object it is
	ObjectLocation mObjectLocation;	///< where the object is

	MatterState mState;	///< The current state of this item

	/// makes sure the item is in the correct state for its temperature
	void checkMatterState();

	/// alters the state of the item
	void changeState(MatterState newState);

	/// used for string-to-state mapping when loading
	void setMatterState(const std::string &state);

	int mSolidToLiquidTemp;	///< Temperature at which this item becomes Liquid
	int mLiquidToGasTemp;	///< Temperature at which this item becomes Gas
	int mGasToPlasmaTemp;	///< Temerature at which this item becomes Plasma

	bool mDestroyedIfNotSolid;	///< Whether this item is destroyed when it reaches Liquid temperature

	int mMagneticStrength; ///< how strong of a magnet is this item?
};

#endif // MUD_PHYSICAL_H
