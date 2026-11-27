#include "Assets.h"

namespace
{
	const char* const kFiles[] =
	{
		"assets/floor.png",
		"assets/wall.png",
		"assets/exit.png",
		"assets/hero.png",
		"assets/skeleton.png",
		"assets/items.png"
	};

	// Order matches Assets::Id. 0 = single picture, 64 = LPC sprite sheet.
	const int kFrameSizes[] = { 0, 0, 0, 64, 64, 16 };
}

Assets::Assets()
	: fLoaded{}
{
}

void Assets::loadAll()
{
	for (size_t i = 0; i < COUNT; i++)
	{
		fLoaded[i] = fTextures[i].loadFromFile(kFiles[i]);
		fTextures[i].setSmooth(false);      // keep pixel art sharp when scaled up
	}
}

bool Assets::has(Id aId) const
{
	return fLoaded[static_cast<size_t>(aId)];
}

const sf::Texture& Assets::get(Id aId) const
{
	return fTextures[static_cast<size_t>(aId)];
}

int Assets::frameSize(Id aId) const
{
	return kFrameSizes[static_cast<size_t>(aId)];
}
