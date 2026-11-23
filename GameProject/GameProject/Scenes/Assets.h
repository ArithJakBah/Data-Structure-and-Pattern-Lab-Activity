#pragma once
#include <cstddef>
#include <SFML/Graphics.hpp>

// Loads every image once and hands out textures by id.
// If a file is missing, has() is false and the scenes draw plain shapes instead.
class Assets
{
public:
	enum class Id { Floor, Wall, Exit, Hero, Skeleton, Count };

private:
	static constexpr size_t COUNT = static_cast<size_t>(Id::Count);

	sf::Texture fTextures[COUNT];
	bool        fLoaded[COUNT];

public:
	Assets();

	void loadAll();                       // reads assets/floor.png, wall.png, exit.png, hero.png, goblin.png
	bool has(Id aId) const;
	int  frameSize(Id aId) const;         // 0 = the whole image is one picture, otherwise the size of one square frame
	const sf::Texture& get(Id aId) const;
};
