#include <set>

#include "zoneMap.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Currently does nothing
*/
ZoneMap::ZoneMap() {
	mHasWeather = false;
}

/// Destructor
/** Currently does nothing
*/
ZoneMap::~ZoneMap() {
}

/// Assigns a map to this object
/** This function expects a character map representation of the zone. This map is a plain text file located in the
	\c data/zones/zone-name directory. The map will be converted to a character array for easier access.
	@param zone The map file from the zone's direcotry
*/
void ZoneMap::setMap(const StringVector &zone) {
	std::vector<char> line;

	for(std::string::size_type i=0; i<zone.size(); ++i) {
		// always make sure to skip comments
		if(zone[i][0] == '#') {
			continue;
		}

		for(unsigned int j=0; j<zone[i].size(); ++j) {
			line.push_back(zone[i].at(j));
		}

		mMap.push_back(line);
		line.clear();
	}

	glob.log.debug(boost::format("ZoneMap::setMap(): Loaded map for zone %1%, it has %2% lines") % mName % mMap.size());
}

/// Gets a nice colored string for the specified coordinate
/** This function creates a string that contains a color code (if specified in the .key file), the terrain character,
	and the color reset code. It's probably only useful to other internal functions such as getRadiusMap(). If no color
	is defined in the key, it simply returns the character at that map position, or an empty string if the request was
	out of range.
	@param x An x coordinate on the map
	@param y A y coordinate on the map
	\return A string formatted version of the colored terrain character or an empty string
*/
std::string ZoneMap::getLocation(const unsigned int x, const unsigned int y) const {
	if(x >= getMaxX() || y >= getMaxY()) {
		glob.log.error(boost::format("ZoneMap::getLocation(): Zone '%3%' x (%1%) or y (%2%) value out of range") % x % y % mName);
		return "";
	}

	// yes, this looks backwards, but it isn't
	char c = mMap[y][x];

	// skip everything if this is a blank space
	if(c == ' ') {
		return " ";
	}

	std::stringstream s;

	std::map<char, std::string>::const_iterator pos = mMapKeyColor.find(c);

	if(pos != mMapKeyColor.end()) {
		s << pos->second;
	} else {
		glob.log.warn(boost::format("ZoneMap::getLocation(): No key color defined for %1% in zone %2%") % c % mName);
		s << c;
	}

	return s.str();
}

/// Fetches a localized map of \c radius blocks around the specified coordinates
/** This function generates a nice color map (if a key is present) of \c radius blocks
	around the given coordinates. Edge cases for \c x and \c y locations will show as much
	as possible while still adhering to the radius.
	@param _x An x coordinate of the map
	@param _y A y coordinate of the map
	@param radius How big a radius around the point you wish the map to be
	@param showLegend Whether or not to show a legend describing the terrain
	\return A string ready to dump to the client
*/
std::string ZoneMap::getRadiusMap(const unsigned int _x, const unsigned int _y, const unsigned int radius, bool showLegend) const {
	if(radius == 0) {
		glob.log.error("ZoneMap::getRadiusMap(): A map was requested with a radius of 0");
		return "";
	}

	std::stringstream s;

	if(_x >= getMaxX()) {
		return "Map error X";
	}

	if(_y >= getMaxY()) {
		return "Map error Y";
	}

	// compute lowest x and y coordinate
	unsigned int startX = _x >= radius ? _x - radius : 0;
	unsigned int startY = _y >= radius ? _y - radius : 0;

	// compute highest x and y coordinate
	unsigned int endX = _x + radius;
	unsigned int endY = _y + radius;

	if(endX >= getMaxX()) {
		endX = getMaxX() - 1;
	}

	if(endY >= getMaxY()) {
		endY = getMaxY() - 1;
	}

	std::set<char> legendSet;

	// loop through the y map lines and extract the x ranges
	for(unsigned int y = startY; y <= endY; ++y) {
		for(unsigned int x = startX; x <= endX; ++x) {
			if(x == _x && y == _y) {
				// this is the player's location
				s << "~rev@~res";
			} else {
				s << getLocation(x, y);
				// only add to the set if we're going to display it to the client
				// this potentially means the client misses something on the map, but the client is IN
				// the location they're missing
				if(mMap[y][x] != ' ') { // skip any empty spaces, they can't be rooms or terrain...
					legendSet.insert(mMap[y][x]);
				}
			}
		}
		s << std::endl;
	}

	if(showLegend) {
		s << std::endl;
		for(std::set<char>::const_iterator it=legendSet.begin(); it != legendSet.end(); ++it) {
			std::map<char, std::string>::const_iterator pos = mMapKeyColor.find(*it);

			if(pos != mMapKeyColor.end()) {
				s << pos->second << "  ";
			} else {
				glob.log.warn(boost::format("ZoneMap::getRadiusMap(): Zone %1%'s map symbol %2% has no color text") % mName % *it);
				s << *it << "  ";
			}

			pos = mMapKeyText.find(*it);

			if(pos != mMapKeyText.end()) {
				s << pos->second;
			} else {
				glob.log.warn(boost::format("ZoneMap::getRadiusMap(): Zone %1%'s map symbol %2% has no legend text") % mName % *it);
				s << "Unspecified";
			}

			s << std::endl;
		}
	}

	return s.str();
}

/// sets the key for any map in this zone
/** This function takes a zone key (read from the zone directory) and loads it in to memory,
	preparing data structures for easy formatting of maps, weather and their respective legends later.
	@param data The contents of a .key file
	@param keyName The name of the map to which the key applies ("map" or "weather")
*/
void ZoneMap::setKey(const StringVector &data, const std::string &keyName) {
	glob.log.debug(boost::format("Parsing %1% key for zone %2%") % keyName % mName);

	if(data.size() == 0) {
		glob.log.warn(boost::format("ZoneMap::setKey(): Received an empty key for zone %1%'s %2% ") % mName % keyName);
		return;
	}

	StringVector temp;

	for(unsigned int i=0; i<data.size(); ++i) {
		if((data[i]).length() == 0) {
			glob.log.error(boost::format("ZoneMap::setKey(): Encountered a blank line for %1% key in zone %2%. Did you forgot to call Utility::stripCommentedLines()?") % keyName % mName);
			continue;
		}

		// keys are char, desc, color
		temp = Utility::stringToVector(data[i], ",");

		if(temp.size() < 2 || temp.size() > 3) {
			glob.log.error(boost::format("ZoneMap::setKey(): Got the wrong number of keys %1% for key %2% in zone %3%") % temp.size() % keyName % mName);
			continue;
		}

		// don't set up any keys unless we have all the parts
		if(temp[0].length() != 1) {
			glob.log.error(boost::format("ZoneMap::setKey(): Key character too long: %1%") % data[i]);
			continue;
		}
		if(temp[1].empty()) {
			glob.log.error(boost::format("ZoneMap::setKey(): Key description is empty: %1%") % data[i]);
			continue;
		}
		// temp[2] can be empty, signifying no color change

		if(keyName == "map") {
			mMapKeyText.insert(std::make_pair(temp[0][0], temp[1]));

			if(temp.size() == 3) {
				mMapKeyColor.insert(std::make_pair(temp[0][0], boost::str(boost::format("%1%%2%~res") % temp[2] % temp[0][0])));
			} else {
				// no color argument
				mMapKeyColor.insert(std::make_pair(temp[0][0], temp[0]));
			}
		} else if(keyName == "weather") {
			mWeatherKeyText.insert(std::make_pair(temp[0][0], temp[1]));

			if(temp.size() == 3) {
				mWeatherKeyColor.insert(std::make_pair(temp[0][0], boost::str(boost::format("%1%%2%~res") % temp[2] % temp[0][0])));
			} else {
				mWeatherKeyColor.insert(std::make_pair(temp[0][0], temp[0]));
			}
		} else {
			glob.log.error(boost::format("ZoneMap::setKey(): Unrecognized key name %1% for zone %2%!") % keyName % mName);
		}

		temp.clear();
	}

	glob.log.debug(boost::format("ZoneMap::setKey(): Finished parsing %1% key for zone %2%") % keyName % mName);
}

/// Formats a nicely colorized map symbol and its description for use in creating a map legend
/** This function formats a string in such a way as to allow you to create a nicely colored
	map legend.
	@param c The map symbol for which you want to display the legend
	\return A std::string with the legend for the specified symbol, or as much as can be generated
*/
std::string ZoneMap::getMapLegendFor(const char c) const {
	std::stringstream s;

	std::map<char, std::string>::const_iterator pos = mMapKeyColor.find(c);

	if(pos != mMapKeyColor.end()) {
		s << pos->second << " ";

		// reuse the iterator since the types are the same
		pos = mMapKeyText.find(c);

		if(pos != mMapKeyText.end()) {
			s << pos->second;
		} else {
			glob.log.error(boost::format("ZoneMap::getMapLegendFor(): No key text present for symbol %1%") % c);
			s << "Unspecified";
		}
	} else {
		glob.log.error(boost::format("ZoneMap::getMapLegendFor(): No color text present for symbol %1%") % c);
		s << c << " Not present";
	}

	return s.str();
}
/// Formats a nicely colorized weather symbol and its description for use in creating a weather legend
/** This function formats a string using the weather key as a guide to display an explanation of the
	different weather symbols, with color.
	@param c The weather symbol the legend should contain
	\return A std::string with the legend for the specified symbol, or as much as can be generated
*/
std::string ZoneMap::getWeatherLegendFor(const char c) const {
	if(!mHasWeather) {
		return "There is no weather here";
	}

	std::stringstream s;

	std::map<char, std::string>::const_iterator pos = mWeatherKeyColor.find(c);

	if(pos != mWeatherKeyColor.end()) {
		s << pos->second << " ";

		// reuse the iterator since the types are the same
		pos = mWeatherKeyText.find(c);

		if(pos != mWeatherKeyText.end()) {
			s << pos->second;
		} else {
			glob.log.error(boost::format("ZoneMap::getWeatherLegendFor(): No key text present for symbol %1%") % c);
			s << "Unspecified";
		}
	} else {
		glob.log.error(boost::format("ZoneMap::getWeatherLegendFor(): No color text present for symbol %1%") % c);
		s << c << " Not present";
	}

	return s.str();
}

/// gets a text explanation of a weather character
/** This function returns the key text value for a given weather character. This does not return a
	colored key for use as a map key
	\see getWeatherLegendFor(const char c)
	@param weather the weather character
	\return a string describing the weather
*/
std::string ZoneMap::getWeatherCharString(const char weather) const {
	if(!mHasWeather) {
		return "There is no weather here";
	}

	std::map<char, std::string>::const_iterator pos = mWeatherKeyText.find(weather);

	if(pos != mWeatherKeyText.end()) {
		return pos->second;
	} else {
		glob.log.warn(boost::format("ZoneMap::getWeatherCharString: No key text present for symbol %1%") % weather);
		return "Unspecified";
	}
}

/// Loads the weather for this zone in to memory
/** This function takes the contents of a weather file for a zone and loads it into a special data structure that
	can be shifted in any direction or distance. It uses a two
	@param wmap The weather map
*/
void ZoneMap::setWeatherMap(StringVector &wmap) {
	std::deque<char> line;

	for(StringVector::iterator it = wmap.begin(); it != wmap.end(); ++it) {
		if((*it).length() == 0 || (*it)[0] == ';') {
			// skip blank lines and comments
			continue;
		}

		for(std::string::size_type j=0; j<(*it).length(); ++j) {
			line.push_back((*it)[j]);
		}

		mWeather.push_back(line);

		if(line.size() != getMaxX()) {
			glob.log.error(boost::format("ZoneMap::setWeatherMap(): Weather map X-size %1% doesn't correlate with map X-size %2%") % line.size() % getMaxX());
		}

		line.clear();
	}

	if(mWeather.size() != getMaxY()) {
		glob.log.error(boost::format("ZoneMap::setWeatherMap(): Weather map Y-size %1% doesn't correlate with map Y-size %2%") % mWeather.size() % getMaxY());
	}

	mHasWeather = true;
}

/// Sets the strength of the wind, which affects how fast the weather map moves
/** This function sets the speed at which the weather shifts across the map.
	@param s The speed of the wind, from 0 to 5
*/
void ZoneMap::setWindStrength(const unsigned int s) {
	if(!mHasWeather) {
		return;
	}

	unsigned int highestAllowed = 5;

	if(s > highestAllowed) {
		glob.log.warn(boost::format("ZoneMap::setWindStrength(): Value %1% not accepted, setting to highest allowed value %2% in zone %3%") % s % highestAllowed % mName);
		mWindStrength = highestAllowed;
	} else {
		mWindStrength = s;
	}
}

/// Returns a human-readable string describing the weather at a certain location
/** This function looks up a location on the weather map that correlates to a location on the main map
	and returns a string describing what the weather is like at that location.
	@param x An x-coordinate
	@param y A y-coordinate
	\return A human-readable std::string describing the weather
*/
std::string ZoneMap::getWeatherString(const int x, const int y) const {
	if(!mHasWeather) {
		return "";
	}

	std::stringstream s;

	s << "The weather here is " << getWeatherCharString(mWeather[y][x]) << '.';

	return s.str();
}

/// generates a string describing the wind
/** This function returns a nice, human-readable string describing the wind, how string it is and
	which way it is blowing
	\return a string describing the wind strength and direction
*/
std::string ZoneMap::getWindString() const {
	if(!mHasWeather) {
		return "";
	}

	std::stringstream s;

	if(mWindStrength == 0) {
		s << "The wind is calm.";
		return s.str();
	}

	switch(mWindStrength) {
		case 1:
			s << "The wind blows gently ";
			break;
		case 2:
			s << "The wind blows lightly ";
			break;
		case 3:
			s << "The wind blows swiftly ";
			break;
		case 4:
			s << "The wind blows strongly ";
			break;
		case 5:
			s << "A gale-force wind is blowing ";
			break;
		default:
			glob.log.warn(boost::format("ZoneMap::getWindStrength(): No case for wind strength %1% in zone %2%") % mWindStrength % mName);
	}

	s << "to the ";

	switch(mWindDirection) {
		case North:
			s << "north";
			break;
		case Northnortheast:
			s << "north-northeast";
			break;
		case Northeast:
			s << "northeast";
			break;
		case Eastnortheast:
			s << "east-northeast";
			break;
		case East:
			s << "east";
			break;
		case Eastsoutheast:
			s << "east-southeast";
			break;
		case Southeast:
			s << "southeast";
			break;
		case Southsoutheast:
			s << "south-southeast";
			break;
		case South:
			s << "south";
			break;
		case Southsouthwest:
			s << "south-southwest";
			break;
		case Southwest:
			s << "southwest";
			break;
		case Westsouthwest:
			s << "west-southwest";
			break;
		case West:
			s << "west";
			break;
		case Westnorthwest:
			s << "west-northwest";
			break;
		case Northwest:
			s << "northwest";
			break;
		case Northnorthwest:
			s << "north-northwest";
			break;
		default:
			glob.log.error(boost::format("ZoneMap::getWindString(): No case for direction %1% in zone %2%") % mWindDirection % mName);
	}

	s << '.';

	return s.str();
}

/// Changes the direction and speed of the wind at random
/** This function determines a new speed and direction for the wind at random. To set it
	specifically, use the overloaded changeWind(Direction, unsigned int) function.
*/
void ZoneMap::changeWind() {
	if(!mHasWeather) {
		return;
	}

	Direction newDir;

	int d = glob.RNG.customInt(1, 16);

	switch(d) {
		case 1: newDir = North; break;
		case 2: newDir = Northnortheast; break;
		case 3: newDir = Northeast; break;
		case 4: newDir = Eastnortheast; break;
		case 5: newDir = East; break;
		case 6: newDir = Eastsoutheast; break;
		case 7: newDir = Southeast; break;
		case 8: newDir = Southsoutheast; break;
		case 9: newDir = South; break;
		case 10: newDir = Southsouthwest; break;
		case 11: newDir = Southwest; break;
		case 12: newDir = Westsouthwest; break;
		case 13: newDir = West; break;
		case 14: newDir = Westnorthwest; break;
		case 15: newDir = Northwest; break;
		case 16: newDir = Northnorthwest; break;
		default:
			newDir = North;
			glob.log.error(boost::format("ZoneMap::changeWind(): unrecognized result %1% returned from RNG") % d);
	}

	setWindStrength(glob.RNG.d6() - 1);
	setWindDirection(newDir);
}

/// Changes the wind and speed to specified values
/** This function changes the wind and wind speed to the values provided, as long as there
	is weather to begin with.
	@param dir The direction for the new wind to blow in. Found in mudconfig.h
	@param strength The strength of the wind, 0 is no wind
*/
void ZoneMap::changeWind(Direction dir, unsigned int strength) {
	if(!mHasWeather) {
		return;
	}

	setWindStrength(strength);
	setWindDirection(dir);
}

/// a simplified function to help move the weather map
/** This utility function makes it a little less typographically complicated to move the
	weather map around.
	@param x How far to shift the map horizontally, positive is right-shift
	@param y How far to shift the map vertically, positive is upward-shift
*/
void ZoneMap::shiftWeather(const int x, const int y) {
	if(x != 0) {
		h_shift(mWeather, x);
	}

	if(y != 0) {
		v_shift(mWeather, y);
	}
}

/// Scrolls the weather map according to the current values
/** This function shifts the weather map around according to the direction of the wind and
	the strength at which it blows.
*/
void ZoneMap::doWeather() {
	if(!mHasWeather) {
		return;
	}

	if(mWindStrength == 0) {
		return;
	}

	switch(mWindDirection) {
		case North:
			shiftWeather(0, 2);
			break;
		case Northnortheast:
			shiftWeather(1, 2);
			break;
		case Northeast:
			shiftWeather(2, 2);
			break;
		case Eastnortheast:
			shiftWeather(2, 1);
			break;
		case East:
			shiftWeather(2, 0);
			break;
		case Eastsoutheast:
			shiftWeather(2, -1);
			break;
		case Southeast:
			shiftWeather(2, -2);
			break;
		case Southsoutheast:
			shiftWeather(1, -2);
			break;
		case South:
			shiftWeather(0, -2);
			break;
		case Southsouthwest:
			shiftWeather(-1, -2);
			break;
		case Southwest:
			shiftWeather(-2, -2);
			break;
		case Westsouthwest:
			shiftWeather(-2, -1);
			break;
		case West:
			shiftWeather(-2, 0);
			break;
		case Westnorthwest:
			shiftWeather(-2, 1);
			break;
		case Northwest:
			shiftWeather(-2, 2);
			break;
		case Northnorthwest:
			shiftWeather(-1, 2);
			break;
		default:
			glob.log.error(boost::format("ZoneMap::doWeather(): No case for direction %1% in zone %2%") % mWindDirection % mName);
	}
}

/// moves a weather map horizontally
/** This function moves a weather map in a positive or negative direction
	horizontally. It should not be called directly.
	@param m A non-const reference to a weather map
	@param distance The distance to shift the map, positive for right shift
*/
void ZoneMap::h_shift(std::deque<std::deque<char> > &m, const int distance) {
	for(std::deque<std::deque<char> >::iterator it = m.begin(); it != m.end(); ++it) {
		if(distance > 0) {
			// shift right
			for(int i=0; i<distance; ++i) {
				char c = (*it).back();
				(*it).pop_back();
				(*it).push_front(c);
			}
		} else {
			// shift left
			for(int i=0; i>distance; --i) {
				char c = (*it).front();
				(*it).pop_front();
				(*it).push_back(c);
			}
		}
	}
}

/// moves a weather map vertically
/** This function moves a weather map in a positive or negative direction
	vertically. It should not be called directly.
	@param m A non-const reference to a weather map
	@param distance The distance to shift the map, positive for upward-shift
*/
void ZoneMap::v_shift(std::deque<std::deque<char> > &m, const int distance) {
	std::deque<char> line;

	if(distance > 0) {
		// shift up
		for(int i=0; i<distance; ++i) {
			line = m.front();
			m.pop_front();
			m.push_back(line);
			line.clear();
		}
	} else {
		// shift down
		for(int i=0; i>distance; --i) {
			line = m.back();
			m.pop_back();
			m.push_front(line);
			line.clear();
		}
	}
}

/// Creates a saveable string to store the current weather condition
/** This function walks through the current weather map and returns it as a string
	that can be saved over the top of the old weather file to preserve its state.
*/
std::string ZoneMap::getWeatherSaveString() const {
	std::stringstream s;

	s << "# Weather map for zone " << mName << std::endl;
	s << "#" << std::endl;
	s << "# DO NOT modify this file while the server is running, your changes will be clobbered!" << std::endl;
	s << "# DO NOT add comments to this file, they will not be preserved!" << std::endl;

	for(std::deque<std::deque<char> >::const_iterator it = mWeather.begin(); it != mWeather.end(); ++it) {
		for(std::deque<char>::const_iterator j = (*it).begin(); j != (*it).end(); ++j) {
			s << *j;
		}
		s << std::endl;
	}

	return s.str();
}

/// gets the character representation for the current weather
/** This function gets the weather character for the specified x,y coordinate
	if there is weather available. If not, it return a NULL character.
	@param x The x-coordinate
	@param y The y-coordinate
	\return the character representation of the weather, or a NULL character
*/
char ZoneMap::getWeatherChar(const int x, const int y) const {
	if(mHasWeather) {
		return mWeather[y][x];
	} else {
		return 0;
	}
}
