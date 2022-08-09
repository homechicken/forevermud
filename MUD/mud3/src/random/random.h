// random.h

#ifndef RANDOM_H
#define RANDOM_H

#include <boost/random.hpp>

/// handles random number generation
/** This class generates random numbers using boost::random. Default functions
	are provided for standard die rolls. A \b customInt() function is provided
	to generate random numbers in a custom range.
	\note This generates <b>evenly distributed</b> random numbers over time. It's
		about as fair as you can get.
*/
class Random {
public:
	Random();
	~Random();

	/// coin flip
	int d2()	{ return mDie2(); }

	/// generates values 1-4
	int d4()	{ return mDie4(); }

	/// generates values 1-6
	int d6()	{ return mDie6(); }

	/// generates values 1-8
	int d8()	{ return mDie8(); }

	/// generates values 1-10
	int d10()	{ return mDie10(); }

	/// generates values 1-12
	int d12()	{ return mDie12(); }

	/// generates values 1-20
	int d20()	{ return mDie20(); }

	/// generates values 1-100
	int d100()	{ return mDie100(); }

	int customInt(int low, int high);

private:
	boost::mt19937 mRNG;

	boost::uniform_int<> mTwo;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > mDie2;

	boost::uniform_int<> mFour;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > mDie4;

	boost::uniform_int<> mSix;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > mDie6;

	boost::uniform_int<> mEight;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > mDie8;

	boost::uniform_int<> mTen;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > mDie10;

	boost::uniform_int<> mTwelve;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > mDie12;

	boost::uniform_int<> mTwenty;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > mDie20;

	boost::uniform_int<> mHundred;
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > mDie100;

};

#endif // RANDOM_H
