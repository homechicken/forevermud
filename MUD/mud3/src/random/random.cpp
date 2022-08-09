#include "random.h"
#include <ctime>

/// initializes variate generators
/** This constructor initializes each of the variate generators for the
	random number functions.
	\note Like most RNGs, this one seeds with the current time
*/
Random::Random() :	mTwo(0,1), mDie2(mRNG, mTwo),
					mFour(1,4), mDie4(mRNG, mFour),
					mSix(1,6), mDie6(mRNG, mSix),
					mEight(1,8), mDie8(mRNG, mEight),
					mTen(1,10), mDie10(mRNG, mTen),
					mTwelve(1,12), mDie12(mRNG, mTwelve),
					mTwenty(1,20), mDie20(mRNG, mTwenty),
					mHundred(1,100), mDie100(mRNG, mHundred)
{
	mRNG.seed(time(NULL));
}

/// destructor
/** This function does nothing
*/
Random::~Random() {
}

/// generates a random number in a custom range
/** This function creates a new generator for a one-time use to
	create a random number in a specified range.
	@param low the lowest value possible
	@param high the highest value possible
	\return a random number between low and high, or 0 if the range is goofy
*/
int Random::customInt(int low, int high) {
	if(low > high || high < 1) {
		return 0;
	}

	boost::uniform_int<> ui(low, high);

	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > custom(mRNG, ui);

	return custom();
}
