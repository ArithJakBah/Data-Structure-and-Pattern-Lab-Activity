#include "ExploreScene.h"
#include <string>
#include "Game.h"
#include "Skeleton.h"
#include "LpcSheet.h"

namespace
{
	// '#' wall   '.' floor   'E' exit
	const char* const kMap[ExploreScene::ROWS] =
	{
		"############",
		"#..........#",
		"#..........#",
		"#...##.....#",
		"#..........E",
		"#...##.....#",
		"#..........#",
		"#..........#",
		"############"
	};

	int lpcDirectionOf(ExploreScene::Direction aDirection)
	{
		switch (aDirection)
		{
		case ExploreScene::Direction::Up:    return Lpc::Up;
		case ExploreScene::Direction::Left:  return Lpc::Left;
		case ExploreScene::Direction::Down:  return Lpc::Down;
		case ExploreScene::Direction::Right: return Lpc::Right;
		}
		return Lpc::Down;
	}
}

ExploreScene::ExploreScene(Game& aGame)
	: fGame(aGame),
	fPlayerRow(4),
	fPlayerCol(1),
	fFacing(Direction::Right),
	fMoving(false),
	fFromRow(4),
	fFromCol(1),
	fToRow(4),
	fToCol(1),
	fMoveTimer(0.f),
	fHeld(Direction::Right),
	fHolding(false),
	fGuardCount(0)
{
	// Two guards. The top row and the right-hand column are a safe way to the exit.
	addGuard(new Skeleton(), 2, 6, Direction::Down, 3);
	addGuard(new Skeleton(), 6, 8, Direction::Up, 3);
}

ExploreScene::~ExploreScene()
{
	for (size_t i = 0; i < fGuardCount; i++)
	{
		delete fGuards[i].monster;
	}
}

void ExploreScene::deltaOf(Direction aDirection, int& aDRow, int& aDCol)
{
	aDRow = 0;
	aDCol = 0;
	switch (aDirection)
	{
	case Direction::Up:    aDRow = -1; break;
	case Direction::Down:  aDRow = 1;  break;
	case Direction::Left:  aDCol = -1; break;
	case Direction::Right: aDCol = 1;  break;
	}
}

bool ExploreScene::directionOf(sf::Keyboard::Key aKey, Direction& aDirection)
{
	switch (aKey)
	{
	case sf::Keyboard::Key::W:
	case sf::Keyboard::Key::Up:
		aDirection = Direction::Up;
		return true;
	case sf::Keyboard::Key::S:
	case sf::Keyboard::Key::Down:
		aDirection = Direction::Down;
		return true;
	case sf::Keyboard::Key::A:
	case sf::Keyboard::Key::Left:
		aDirection = Direction::Left;
		return true;
	case sf::Keyboard::Key::D:
	case sf::Keyboard::Key::Right:
		aDirection = Direction::Right;
		return true;
	default:
		return false;
	}
}

bool ExploreScene::isWall(int aRow, int aCol) const
{
	if (aRow < 0 || aRow >= ROWS || aCol < 0 || aCol >= COLS)
	{
		return true;
	}
	return kMap[aRow][aCol] == '#';
}

bool ExploreScene::guardAt(int aRow, int aCol) const
{
	for (size_t i = 0; i < fGuardCount; i++)
	{
		if (fGuards[i].monster->isAlive() && fGuards[i].row == aRow && fGuards[i].col == aCol)
		{
			return true;
		}
	}
	return false;
}

int ExploreScene::visionTiles(const Guard& aGuard, int aRows[], int aCols[]) const
{
	int dRow = 0;
	int dCol = 0;
	deltaOf(aGuard.facing, dRow, dCol);

	int count = 0;
	for (int k = 1; k <= aGuard.range && count < MAX_VISION; k++)
	{
		int r = aGuard.row + dRow * k;
		int c = aGuard.col + dCol * k;
		if (isWall(r, c) || guardAt(r, c))
		{
			break;      // walls and other guards block the line of sight
		}
		aRows[count] = r;
		aCols[count] = c;
		count++;
	}
	return count;
}

bool ExploreScene::guardSeesPlayer(const Guard& aGuard) const
{
	int rows[MAX_VISION];
	int cols[MAX_VISION];
	int count = visionTiles(aGuard, rows, cols);
	for (int i = 0; i < count; i++)
	{
		if (rows[i] == fPlayerRow && cols[i] == fPlayerCol)
		{
			return true;
		}
	}
	return false;
}

void ExploreScene::addGuard(Monster* aMonster, int aRow, int aCol, Direction aFacing, int aRange)
{
	if (fGuardCount >= MAX_GUARDS)
	{
		delete aMonster;
		return;
	}
	fGuards[fGuardCount].monster = aMonster;
	fGuards[fGuardCount].row = aRow;
	fGuards[fGuardCount].col = aCol;
	fGuards[fGuardCount].facing = aFacing;
	fGuards[fGuardCount].range = aRange;
	fGuardCount++;
}

void ExploreScene::tryMove(Direction aDirection)
{
	fFacing = aDirection;           // turn even if the way is blocked

	int dRow = 0;
	int dCol = 0;
	deltaOf(aDirection, dRow, dCol);
	int r = fPlayerRow + dRow;
	int c = fPlayerCol + dCol;
	if (isWall(r, c) || guardAt(r, c))
	{
		return;
	}

	fMoving = true;
	fFromRow = fPlayerRow;
	fFromCol = fPlayerCol;
	fToRow = r;
	fToCol = c;
	fMoveTimer = 0.f;
}

void ExploreScene::checkTile()
{
	if (kMap[fPlayerRow][fPlayerCol] == 'E')
	{
		fGame.requestVictory();
		return;
	}

	bool spotted = false;
	for (size_t i = 0; i < fGuardCount; i++)
	{
		if (fGuards[i].monster->isAlive() && guardSeesPlayer(fGuards[i]))
		{
			spotted = true;
		}
	}
	if (!spotted)
	{
		return;
	}

	// One guard raises the alarm: every living guard in the room joins the fight.
	Monster* fighters[Battle::MAX_MONSTERS];
	size_t count = 0;
	for (size_t i = 0; i < fGuardCount && count < Battle::MAX_MONSTERS; i++)
	{
		if (fGuards[i].monster->isAlive())
		{
			fighters[count++] = fGuards[i].monster;
		}
	}
	fGame.requestBattle(fighters, count);
}

void ExploreScene::removeDefeated()
{
	size_t kept = 0;
	for (size_t i = 0; i < fGuardCount; i++)
	{
		if (fGuards[i].monster->isAlive())
		{
			fGuards[kept++] = fGuards[i];
		}
		else
		{
			delete fGuards[i].monster;
		}
	}
	fGuardCount = kept;

	// Key-release events went to the battle screen, so forget any held key.
	fHolding = false;
	fMoving = false;
}

void ExploreScene::handleInput(const sf::Event& aEvent)
{
	if (fGame.hasPendingRequest())
	{
		return;
	}

	if (aEvent.is<sf::Event::FocusLost>())
	{
		fHolding = false;
		return;
	}

	if (const auto* released = aEvent.getIf<sf::Event::KeyReleased>())
	{
		Direction d = Direction::Down;
		if (directionOf(released->code, d) && fHolding && d == fHeld)
		{
			fHolding = false;
		}
		return;
	}

	const auto* key = aEvent.getIf<sf::Event::KeyPressed>();
	if (key == nullptr)
	{
		return;
	}
	if (key->code == sf::Keyboard::Key::Escape)
	{
		fGame.quit();
		return;
	}

	Direction d = Direction::Down;
	if (directionOf(key->code, d))
	{
		fHeld = d;
		fHolding = true;
		if (!fMoving)
		{
			tryMove(d);         // a quick tap always moves one tile
		}
	}
}

void ExploreScene::update(float aDt)
{
	if (fGame.hasPendingRequest())
	{
		return;
	}

	if (fMoving)
	{
		fMoveTimer += aDt;
		if (fMoveTimer >= MOVE_TIME)
		{
			fPlayerRow = fToRow;
			fPlayerCol = fToCol;
			fMoving = false;
			checkTile();
			if (fHolding && !fGame.hasPendingRequest())
			{
				tryMove(fHeld);     // key still held: keep walking without a pause
			}
		}
	}
	else if (fHolding)
	{
		tryMove(fHeld);
	}
}

void ExploreScene::draw(sf::RenderWindow& aWindow)
{
	const float tile = static_cast<float>(TILE);
	sf::RectangleShape rect({ tile - 2.f, tile - 2.f });     // fallback when an image is missing
	sf::RectangleShape overlay({ tile, tile });

	// Floor, walls and exit
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			char t = kMap[r][c];
			Assets::Id id = Assets::Id::Floor;
			if (t == '#')
			{
				id = Assets::Id::Wall;
			}
			else if (t == 'E')
			{
				id = Assets::Id::Exit;
			}

			float x = c * tile;
			float y = r * tile;
			if (!fGame.drawSprite(aWindow, id, x, y, tile, tile))
			{
				if (t == '#')
				{
					rect.setFillColor(sf::Color(90, 90, 110));
				}
				else if (t == 'E')
				{
					rect.setFillColor(sf::Color(60, 160, 80));
				}
				else
				{
					rect.setFillColor(sf::Color(45, 45, 58));
				}
				rect.setPosition({ x + 1.f, y + 1.f });
				aWindow.draw(rect);
			}
		}
	}

	// What each living guard can see (red tiles)
	overlay.setFillColor(sf::Color(220, 60, 60, 90));
	for (size_t i = 0; i < fGuardCount; i++)
	{
		if (!fGuards[i].monster->isAlive())
		{
			continue;
		}
		int rows[MAX_VISION];
		int cols[MAX_VISION];
		int count = visionTiles(fGuards[i], rows, cols);
		for (int k = 0; k < count; k++)
		{
			overlay.setPosition({ cols[k] * tile, rows[k] * tile });
			aWindow.draw(overlay);
		}
	}

	// Guards (they stand still and look the way they face)
	for (size_t i = 0; i < fGuardCount; i++)
	{
		const Guard& g = fGuards[i];
		if (!g.monster->isAlive())
		{
			continue;
		}

		float x = g.col * tile;
		float y = g.row * tile;
		int sheetRow = Lpc::WALK_ROW + lpcDirectionOf(g.facing);      // standing, looking the way it faces
		if (!fGame.drawFrame(aWindow, Assets::Id::Skeleton, 0, sheetRow, x, y, tile))
		{
			rect.setFillColor(sf::Color(200, 120, 40));
			rect.setPosition({ x + 1.f, y + 1.f });
			aWindow.draw(rect);

			// no picture: show the facing with a white "eye" instead
			int dRow = 0;
			int dCol = 0;
			deltaOf(g.facing, dRow, dCol);
			sf::RectangleShape eye({ 8.f, 8.f });
			eye.setFillColor(sf::Color(255, 255, 255, 200));
			eye.setPosition({ x + tile / 2.f - 4.f + dCol * 24.f,
			                  y + tile / 2.f - 4.f + dRow * 24.f });
			aWindow.draw(eye);
		}
	}

	// Player: slides between tiles and plays the walk cycle while moving
	float progress = 0.f;
	float row = static_cast<float>(fPlayerRow);
	float col = static_cast<float>(fPlayerCol);
	int frameCol = 0;                               // frame 0 = standing still
	if (fMoving)
	{
		progress = fMoveTimer / MOVE_TIME;
		if (progress > 1.f)
		{
			progress = 1.f;
		}
		row = fFromRow + (fToRow - fFromRow) * progress;
		col = fFromCol + (fToCol - fFromCol) * progress;
		frameCol = 1 + static_cast<int>(progress * 8.f);
		if (frameCol > Lpc::WALK_FRAMES - 1)
		{
			frameCol = Lpc::WALK_FRAMES - 1;
		}
	}
	float playerX = col * tile;
	float playerY = row * tile;
	int sheetRow = Lpc::WALK_ROW + lpcDirectionOf(fFacing);
	if (!fGame.drawFrame(aWindow, Assets::Id::Hero, frameCol, sheetRow, playerX, playerY, tile))
	{
		rect.setFillColor(sf::Color(70, 130, 230));
		rect.setPosition({ playerX + 1.f, playerY + 1.f });
		aWindow.draw(rect);
	}

	// HUD
	Player& p = fGame.player();
	float hudY = ROWS * tile + 8.f;
	fGame.drawText(aWindow,
		"HP " + std::to_string(p.health()) + "/" + std::to_string(p.maxHealth()) +
		"    MP " + std::to_string(p.mana()) + "/" + std::to_string(p.maxMana()),
		10.f, hudY, 20, sf::Color::White);
	fGame.drawText(aWindow,
		"WASD / arrows: move (hold to walk)    Red tiles = guard sight    Green door = exit    Esc: quit",
		10.f, hudY + 28.f, 15, sf::Color(180, 180, 190));
}
