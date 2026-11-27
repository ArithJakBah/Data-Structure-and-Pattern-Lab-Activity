#pragma once
#include <cstddef>
#include <string>
#include "Scene.h"
#include "Monster.h"
#include "Item.h"

class Game;

// Walking around a tile room. Guards stand still, face a direction and "see" a few tiles ahead.
// Items lie on the floor and are picked up by walking over them. Press I for the inventory.
class ExploreScene : public Scene
{
public:
	static constexpr int    ROWS = 9;
	static constexpr int    COLS = 12;
	static constexpr int    TILE = 64;
	static constexpr size_t MAX_GUARDS = 6;
	static constexpr size_t MAX_FLOOR_ITEMS = 12;
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

	struct FloorItem
	{
		Item* item;
		int   row;
		int   col;
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
	FloorItem fFloor[MAX_FLOOR_ITEMS];
	size_t    fFloorCount;

	bool        fShowInventory;
	int         fInvIndex;
	std::string fInvMessage;
	std::string fToast;
	float       fToastTimer;

	static void deltaOf(Direction aDirection, int& aDRow, int& aDCol);
	static bool directionOf(sf::Keyboard::Key aKey, Direction& aDirection);
	bool isWall(int aRow, int aCol) const;
	bool guardAt(int aRow, int aCol) const;                                  // a living guard stands here
	int  visionTiles(const Guard& aGuard, int aRows[], int aCols[]) const;   // tiles the guard can see
	bool guardSeesPlayer(const Guard& aGuard) const;
	void addGuard(Monster* aMonster, int aRow, int aCol, Direction aFacing, int aRange);
	void addFloorItem(Item* aItem, int aRow, int aCol);
	void pickUpHere();
	void showToast(const std::string& aText);
	void tryMove(Direction aDirection);
	void checkTile();

	void handleInventoryKey(sf::Keyboard::Key aKey);
	void useSelected();
	void unequipSlot(Equipment::Slot aSlot);
	void drawInventory(sf::RenderWindow& aWindow) const;

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
