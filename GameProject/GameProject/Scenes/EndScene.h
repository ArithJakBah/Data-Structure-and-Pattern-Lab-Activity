#pragma once
#include "Scene.h"

class Game;

// Shown after winning or dying.
class EndScene : public Scene
{
private:
	Game& fGame;
	bool  fVictory;

public:
	EndScene(Game& aGame, bool aVictory);

	void handleInput(const sf::Event& aEvent) override;
	void update(float aDt) override;
	void draw(sf::RenderWindow& aWindow) override;
};
