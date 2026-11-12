#pragma once
#include <string>

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
};