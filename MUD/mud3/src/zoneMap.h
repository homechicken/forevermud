#ifndef ZONEMAP_H
#define ZONEMAP_H

#include "mudconfig.h"
#include <vector>
#include <string>
#include <map>
#include <deque>
#include <sstream>

/// Stores map information
/** This class handles all necessary data for a zone, including weather, rooms,
	and the functions necessary to support them.
*/
class ZoneMap {
public:
	ZoneMap();
	~ZoneMap();

	/// sets the name of the zone
	void setName(const std::string &name) { mName = name; }

	/// returns the name of this zone
	std::string getName() const { return mName; }

	void setMap(const StringVector &map);

	void setKey(const StringVector &data, const std::string &keyName);

	void setWeatherMap(StringVector &wmap);

	/// tells whether or not this zone has weather associated with it
	bool hasWeather() const	{ return mHasWeather; }

	/// returns the direction in which the wind blows
	Direction getWindDirection() const { return mWindDirection; }

	/// sets the direction in which the wind is blowing
	void setWindDirection(Direction dir) { mWindDirection = dir; }

	/// returns how strong the wind is blowing
	unsigned int getWindStrength() const { return mWindStrength; }

	void setWindStrength(const unsigned int s);

	std::string getWeatherString(const int x, const int y) const;

	char getWeatherChar(const int x, const int y) const;

	void changeWind();
	void changeWind(Direction dir, unsigned int strength);

	std::string getWindString() const;

	void doWeather();

	std::string getWeatherSaveString() const;

	std::string getLocation(const unsigned int x, const unsigned int y) const;

	//std::string getWeatherAtLocation(const int x, const int y); // can't remember why I added this

	/// returns the width of the terrain map
	unsigned int getMaxX() const { return mMap[0].size(); }

	/// returns the height of the terrain map
	unsigned int getMaxY() const { return mMap.size(); }

	std::string getRadiusMap(const unsigned int x, const unsigned int y, const unsigned int radius, bool showLegend = true) const;

	std::string getMapLegendFor(const char c) const;
	std::string getWeatherLegendFor(const char c) const;
	std::string getWeatherCharString(const char weather) const;

private:
	std::string mName; ///< The name of this zone

	std::vector<std::vector<char> > mMap; ///< Stores the map of the terrain for this zone

	std::map<char, std::string> mMapKeyText; ///< Stores the description of the map symbol, eg. 'Mountains', 'Town'
	std::map<char, std::string> mMapKeyColor; ///< Stores the color code, the map symbol, and the color reset code

	bool mHasWeather; ///< Does this map have an associated weather map?
	std::deque<std::deque<char> > mWeather; ///< Stores the weather map for this zone

	std::map<char, std::string> mWeatherKeyText; ///< Stores the description of the weather symbol
	std::map<char, std::string> mWeatherKeyColor; ///< Stores the color code, symbol, and reset code for each weather type

	Direction mWindDirection; ///< Stores which direction the wind is blowing
	unsigned int mWindStrength; ///< Stores how hard the wind is blowing, restricted to 0-3

	void h_shift(std::deque<std::deque<char> > &m, const int distance);
	void v_shift(std::deque<std::deque<char> > &m, const int distance);
	void shiftWeather(const int x, const int y);

};


#endif // ZONEMAP_H
