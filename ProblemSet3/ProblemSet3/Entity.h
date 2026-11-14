#pragma once
#include <string>

class Visitor; //foward declaration

class Entity
{
private:
	std::string fName;
	std::string fDescription;

public:
	Entity (const std::string& aName, const std::string& aDescription) 
		noexcept: fName( aName), fDescription( aDescription ) {}

	virtual ~Entity() = default;

	const std::string& name() const noexcept { return fName; }
	const std::string& description() const noexcept { return fDescription; }

	// Visitor hook. Visitable subclasses override this; the default does nothing.
	virtual void accept(Visitor&) {}

	// True once the entity is used up and should be removed from its room.
	virtual bool isGone() const noexcept { return false; }
};