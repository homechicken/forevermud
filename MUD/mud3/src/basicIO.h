#ifndef MUD_BASIC_IO_H
#define MUD_BASIC_IO_H

/// Basic, virtual I/O class
/** This class is meant to be purely virtual, use it to build classes that
	need to store data offline. */
class BasicIO {
public:
	/// Constructor, nothing special here
	BasicIO() {};

	/// Destructor, nothing special here either
	virtual ~BasicIO() {};

	/// retrieves the internal name of the resource
	std::string getResourceName() const { return mResourceName; }

	/// sets the internal name of the resource
	void setResourceName(const std::string &name) { mResourceName = name; }

	/// pure virtual load function to be overloaded by the inheritor
	virtual bool load() = 0;

	/// pure virtual save function to be overloaded by the inheritor
	virtual bool save() = 0;
private:
	std::string mResourceName;	///< identifies the resource to work with (file, database, etc)
};

#endif // MUD_BASIC_IO_H
