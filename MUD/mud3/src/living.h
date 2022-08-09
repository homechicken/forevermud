#ifndef MUD_LIVING_H
#define MUD_LIVING_H

#include <yaml-cpp/yaml.h>
#include "mudconfig.h"
#include "physical.h"

/// a class that describes a living object
/** This class defines what a living object is
*/
class Living : public Physical{
public:
	/// defines the different sexes available
	typedef enum {
		Male = 0,
		Female,
		Neuter
	} Sex;

	/// defines the different states a living may be in
	typedef enum {
		Alive = 0,
		Unconscious,
		Dead,
		Ghost
	} LivingState;

	/// defines the different postures a living may be in
	typedef enum {
		Standing = 0,
		Kneeling,
		Sitting,
		Lying,
		Prone
	} LivingPosture;


	Living();
	~Living();

	/// gets the amount of life left
	int getLife() const { return mLife; }

	/// gets the maximum amount of life
	int getMaxLife() const { return mMaxLife; }
	/// sets the maximum amount of life
	void setMaxLife(const int life) { mMaxLife = life; }

	/// gets the sex of the Living object
	Sex getSex() const { return mSex; }
	void setSex(Sex sex);

	bool addLife(const int amount);
	bool subtractLife(const int amount);

	/// sets the LivingState of the Living object
	LivingState livingGetState() const { return mLivingState; }
	std::string livingGetStateString() const;

	bool livingRevive();

	/// returns the LivingPosture of the Living object
	LivingPosture livingGetPosture() const { return mLivingPosture; }
	std::string livingGetPostureString() const;
	bool livingSetPosture(LivingPosture posture);

	void livingSave(YAML::Emitter &out) const;
	bool livingLoad(const YAML::Node &node);

	/// returns the lowest comfortable temperature for this living
	int getTempComfortLow() const { return mTempComfortLow; }
	/// returns the highest comfortable temperature for this living
	int getTempComfortHigh() const { return mTempComfortHigh; }
	/// returns the temp at which cold damages this living
	int getTempDamageLow() const { return mTempDamageLow; }
	/// returns the temp at which heat damages this living
	int getTempDamageHigh() const { return mTempDamageHigh; }

	/// returns the possessive pronoun for this living
	std::string getPossessivePronoun() const { return mPossessivePronoun; }

private:
	int mLife;		///< current life value
	int mMaxLife;	///< maximum amount of life
	std::string mRace;	///< what race this object is
	Sex mSex;		///< Sex of this object
	std::string mPossessivePronoun;	///< the relevant possessive pronoun for this sex
	LivingState mLivingState;		///< LivingState this Living is in
	LivingPosture mLivingPosture;	///< LivingPosture this Living is in

	int mTempComfortLow;	///< lowest comfortable temperature
	int mTempComfortHigh;	///< highest comfortable temperature
	int mTempDamageLow;		///< low temp threshold at which damage begins
	int mTempDamageHigh;	///< high temp threshold at which damage begins
};

#endif // MUD_LIVING_H
