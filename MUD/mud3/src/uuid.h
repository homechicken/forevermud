#ifndef MUD_UUID_H
#define MUD_UUID_H

#include <string>

#include <boost/config/warning_disable.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

/** A Singleton version of a Universally Unique ID generator
*/
class UUID {
public:
	boost::uuids::uuid create() { return mRandomGenerator(); }

	boost::uuids::uuid createFromString(const std::string &data) { return mStringGenerator(data); }

	boost::uuids::uuid createNull() { return boost::uuids::nil_uuid(); }

	bool isNull(const boost::uuids::uuid &id) { return id.is_nil(); }

	std::string toString(const boost::uuids::uuid &id);

	static UUID* Instance();
	
protected:
	virtual ~UUID();

private:
	// constructors and assignment operators are private
	UUID() {}

	UUID(const UUID &);
	UUID& operator=(const UUID &);

	boost::uuids::string_generator mStringGenerator;
	boost::uuids::random_generator mRandomGenerator;

};

#endif // MUD_UUID_H
