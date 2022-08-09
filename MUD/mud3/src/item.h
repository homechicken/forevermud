#ifndef MUD_ITEM_H
#define MUD_ITEM_H

#include "mudconfig.h"
#include "physical.h"

/// a class for virtual items in the world
/** This class stores information and functions that are needed for any virtual
	item to be found in the world.
*/
class Item {
public:
	/// defines the different states an item may exist in
	typedef enum {
		Invalid = 0,
		Destruct,
		Solid,
		Liquid,
		Gas,
		Plasma
	} ItemPhysicalState;

	Item();
	~Item();

	/// Returns the current PhysicalState the item is in
	ItemPhysicalState getState() const { return mState; }

	/// Whether or not you want to notify the containing object on PhysicalState change
	void setMessageOnStateChange(bool m)	{ mMessageOnStateChange = m; }
	bool getMessages(std::string &msg);

	/// Returns the temperature at which this item turns to a liquid
	int getSolidToLiquidTemp() const { return mSolidToLiquidTemp; }
	/// Sets the temperature at which this item turns to a liquid
	void setSolidToLiquidTemp(const int t) { mSolidToLiquidTemp = t; }

	/// Returns the temperature at which this item turns to a gas
	int getLiquidToGasTemp() const { return mLiquidToGasTemp; }
	/// Sets the temperature at which this item turns to a gas
	void setLiquidToGasTemp(const int t) { mLiquidToGasTemp = t; }

	/// Returns true if this item is destroyed when it reaches Liquid temperature
	bool getDestroyedIfNotSolid() const { return mDestroyedIfNotSolid; }
	/// Sets whether this item is destroyed upon PhysicalState change to Liquid
	void setDestroyedIfNotSolid(bool b)		{ mDestroyedIfNotSolid = b; }

	/// Returns the strength of this item's magnetic field
	int getMagneticStrength() const { return mMagneticStrength; }
	/// Sets this item's magnetic strength
	void setMagneticStrength(const int s) { mMagneticStrength = s; }

	/// Returns the quality of this item
	int getQuality() const { return mQuality; }
	/// Sets the quality of this item
	void setQuality(const int q) { mQuality = q; }

	void checkTemperature(const int temp);

	void itemSave(YAML::Emitter &out) const;

	bool itemLoad(const YAML::Node &node);

	std::string getItemPhysicalStateString() const;
	std::string getItemPhysicalStateString(ItemPhysicalState) const;

	void setItemPhysicalState(const std::string &text);

private:
	ItemPhysicalState mState;	///< The current state of this item
	bool mMessageOnStateChange;	///< Whether this item emits a message when it changes PhysicalState

	void changeState(ItemPhysicalState newState);

	int mSolidToLiquidTemp;	///< Temperature at which this item becomes Liquid
	int mLiquidToGasTemp;	///< Temperature at which this item becomes Gas
	int mGasToPlasmaTemp;	///< Temerature at which this item becomes Plasma

	bool mDestroyedIfNotSolid;	///< Whether this item is destroyed when it reaches Liquid temperature

	int mMagneticStrength; ///< how strong of a magnet is this item?

	int mQuality;	///< how nice is this item?

	std::string mMessage;	///< the message to be emitted if mMessageOnStateChange == true

};

#endif // MUD_ITEM_H
