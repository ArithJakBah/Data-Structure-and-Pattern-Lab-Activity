#pragma once
#include <cstddef>
#include "Scene.h"
#include "Monster.h"

class Game;

// Walking around a tile room. Guards stand still, face a direction and "see" a few tiles ahead.
class ExploreScene : public Scene
{
public:
	static constexpr int    ROWS = 9;
	static constexpr int    COLS = 12;
	static constexpr int    TILE = 64;
	static constexpr size_t MAX_GUARDS = 6;
	static constexpr int    MAX_VISION = 6;
	static constexpr float  MOVE_TIME = 0.18f;     // seconds to walk one tile

	enum class Direction { Up, Down, Left, Right };

private:
	struct Guard
	{
		Monster*  monster;
		int       row;
		int       col;
		Direction facing;
		int       range;
	};

	Game&     fGame;
	int       fPlayerRow;
	int       fPlayerCol;
	Direction fFacing;
	bool      fMoving;           // true while sliding from one tile to the next
	int       fFromRow;
	int       fFromCol;
	int       fToRow;
	int       fToCol;
	float     fMoveTimer;
	Direction fHeld;             // direction key currently held down
	bool      fHolding;
	Guard     fGuards[MAX_GUARDS];
	size_t    fGuardCount;

	static void deltaOf(Direction aDirection, int& aDRow, int& aDCol);
	static bool directionOf(sf::Keyboard::Key aKey, Direction& aDirection);
	bool isWall(int aRow, int aCol) const;
	bool guardAt(int aRow, int aCol) const;                                  // a living guard stands here
	int  visionTiles(const Guard& aGuard, int aRows[], int aCols[]) const;   // tiles the guard can see
	bool guardSeesPlayer(const Guard& aGuard) const;
	void addGuard(Monster* aMonster, int aRow, int aCol, Direction aFacing, int aRange);
	void tryMove(Direction aDirection);
	void checkTile();

public:
	explicit ExploreScene(Game& aGame);
	~ExploreScene() override;
	ExploreScene(const ExploreScene&) = delete;
	ExploreScene& operator=(const ExploreScene&) = delete;

	void removeDefeated();      // called when we come back from a battle

	void handleInput(const sf::Event& aEvent) override;
	void update(float aDt) override;
	void draw(sf::RenderWindow& aWindow) override;
};
