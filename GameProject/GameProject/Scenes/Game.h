#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Scene.h"
#include "Assets.h"
#include "Player.h"
#include "Battle.h"

class ExploreScene;
class BattleScene;
class EndScene;

// Context of the State pattern: owns the window, the player and the current scene.
class Game
{
public:
	static constexpr unsigned WINDOW_WIDTH = 768;
	static constexpr unsigned WINDOW_HEIGHT = 648;

private:
	enum class Request { None, StartBattle, BackToExplore, ShowGameOver, ShowVictory, RestartGame };

	sf::RenderWindow fWindow;
	sf::Font         fFont;
	bool             fFontLoaded;
	Assets           fAssets;
	Player*          fPlayer;

	ExploreScene*    fExplore;
	BattleScene*     fBattle;
	EndScene*        fEnd;
	Scene*           fCurrent;

	Request          fRequest;
	Monster*         fPendingMonsters[Battle::MAX_MONSTERS];
	size_t           fPendingCount;

	void applyRequest();

public:
	Game();
	~Game();
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	void run();

	// Scenes ask for a change; it is applied at the end of the frame,
	// so a scene is never deleted while one of its own functions is running.
	void requestBattle(Monster* aMonsters[], size_t aCount);
	void requestExplore();
	void requestGameOver();
	void requestVictory();
	void requestRestart();
	bool hasPendingRequest() const;
	void quit();

	Player& player();
	void drawText(sf::RenderTarget& aTarget, const std::string& aText,
		float aX, float aY, unsigned aSize, sf::Color aColor) const;

	// Draws an image stretched to aWidth x aHeight. Returns false if the image is missing,
	// so the caller can draw a plain shape instead.
	bool drawSprite(sf::RenderTarget& aTarget, Assets::Id aId,
		float aX, float aY, float aWidth, float aHeight,
		sf::Color aTint = sf::Color::White, bool aFlipX = false) const;

	// Draws one frame (column, row) of a sprite sheet as an aSize x aSize square.
	bool drawFrame(sf::RenderTarget& aTarget, Assets::Id aId, int aCol, int aRow,
		float aX, float aY, float aSize,
		sf::Color aTint = sf::Color::White, bool aFlipX = false) const;
};
