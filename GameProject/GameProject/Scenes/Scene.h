#pragma once
#include <SFML/Graphics.hpp>

class Game;

// State pattern: every screen of the game implements this interface.
class Scene
{
public:
	virtual ~Scene() = default;

	virtual void handleInput(const sf::Event& aEvent) = 0;
	virtual void update(float aDt) = 0;
	virtual void draw(sf::RenderWindow& aWindow) = 0;
};
