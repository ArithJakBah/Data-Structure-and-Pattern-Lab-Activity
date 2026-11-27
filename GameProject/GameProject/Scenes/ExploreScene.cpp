#include "ExploreScene.h"
#include <string>
#include "Game.h"
#include "Skeleton.h"
#include "LpcSheet.h"
#include "Inventory.h"
#include "Skill.h"

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
	fGuardCount(0),
	fFloorCount(0),
	fShowInventory(false),
	fInvIndex(0),
	fToastTimer(0.f)
{
	// Two guards. The top row and the right-hand column are a safe way to the exit.
	addGuard(new Skeleton(), 2, 6, Direction::Down, 3);
	addGuard(new Skeleton(), 6, 8, Direction::Up, 3);

	// Items on the floor. The ones inside a guard's red sight line cost you a fight.
	addFloorItem(new Potion(Potion::Kind::Health, 15), 1, 3);
	addFloorItem(new Weapon("Iron Sword", 3), 6, 1);
	addFloorItem(new Potion(Potion::Kind::Mana, 8), 7, 3);
	addFloorItem(new SkillScroll("Shield"), 1, 10);
	addFloorItem(new KeyFragment(), 3, 10);
	addFloorItem(new SkillScroll("Heal"), 5, 6);               // in the sight line of the first guard
	addFloorItem(new Armor("Chain Mail", 10, 1), 4, 8);        // in the sight line of the second guard
}

ExploreScene::~ExploreScene()
{
	for (size_t i = 0; i < fGuardCount; i++)
	{
		delete fGuards[i].monster;
	}
	for (size_t i = 0; i < fFloorCount; i++)
	{
		delete fFloor[i].item;
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

void ExploreScene::addFloorItem(Item* aItem, int aRow, int aCol)
{
	if (fFloorCount >= MAX_FLOOR_ITEMS)
	{
		delete aItem;
		return;
	}
	fFloor[fFloorCount].item = aItem;
	fFloor[fFloorCount].row = aRow;
	fFloor[fFloorCount].col = aCol;
	fFloorCount++;
}

void ExploreScene::showToast(const std::string& aText)
{
	fToast = aText;
	fToastTimer = 2.5f;
}

void ExploreScene::pickUpHere()
{
	for (size_t i = 0; i < fFloorCount; i++)
	{
		if (fFloor[i].row != fPlayerRow || fFloor[i].col != fPlayerCol)
		{
			continue;
		}

		Item* item = fFloor[i].item;
		if (!fGame.player().pickUp(item))
		{
			showToast("Bag full: can't take " + item->name());
			return;
		}
		showToast("Picked up " + item->name());
		fFloor[i] = fFloor[fFloorCount - 1];        // the bag owns it now; close the gap
		fFloorCount--;
		return;
	}
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

	pickUpHere();

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
	fShowInventory = false;
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

	if (fShowInventory)
	{
		handleInventoryKey(key->code);
		return;
	}
	if (key->code == sf::Keyboard::Key::I)
	{
		if (!fMoving)
		{
			fShowInventory = true;
			fHolding = false;
			fInvIndex = 0;
			fInvMessage.clear();
		}
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
	if (fToastTimer > 0.f)
	{
		fToastTimer -= aDt;
	}
	if (fShowInventory)
	{
		return;         // the game is paused while you look in your bag
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

	// Items lying on the floor
	for (size_t i = 0; i < fFloorCount; i++)
	{
		fGame.drawFrame(aWindow, Assets::Id::Items, fFloor[i].item->iconIndex(), 0,
			fFloor[i].col * tile + 14.f, fFloor[i].row * tile + 14.f, 36.f);
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
		"   MP " + std::to_string(p.mana()) + "/" + std::to_string(p.maxMana()) +
		"   ATK " + std::to_string(p.attack()) + "   DEF " + std::to_string(p.defence()) +
		"   Key fragments " + std::to_string(p.keyFragments()) + "/" + std::to_string(Player::FRAGMENTS_NEEDED),
		10.f, hudY, 20, sf::Color::White);
	fGame.drawText(aWindow,
		"WASD / arrows: move (hold to walk)   I: inventory   Red tiles = guard sight   Esc: quit",
		10.f, hudY + 28.f, 15, sf::Color(180, 180, 190));

	// Short message ("Picked up ...")
	if (fToastTimer > 0.f)
	{
		sf::RectangleShape box({ 460.f, 34.f });
		box.setPosition({ 10.f, 10.f });
		box.setFillColor(sf::Color(20, 20, 32, 220));
		aWindow.draw(box);
		fGame.drawText(aWindow, fToast, 20.f, 14.f, 20, sf::Color(255, 230, 120));
	}

	if (fShowInventory)
	{
		drawInventory(aWindow);
	}
}

// ---------------------------------------------------------------- inventory screen
void ExploreScene::handleInventoryKey(sf::Keyboard::Key aKey)
{
	Player& p = fGame.player();
	int count = static_cast<int>(p.inventory().count());

	switch (aKey)
	{
	case sf::Keyboard::Key::I:
	case sf::Keyboard::Key::Escape:
		fShowInventory = false;
		break;
	case sf::Keyboard::Key::W:
	case sf::Keyboard::Key::Up:
		if (count > 0)
		{
			fInvIndex = (fInvIndex + count - 1) % count;
		}
		break;
	case sf::Keyboard::Key::S:
	case sf::Keyboard::Key::Down:
		if (count > 0)
		{
			fInvIndex = (fInvIndex + 1) % count;
		}
		break;
	case sf::Keyboard::Key::Enter:
	case sf::Keyboard::Key::Space:
		useSelected();
		break;
	case sf::Keyboard::Key::Num1:
		unequipSlot(Equipment::Slot::Weapon);
		break;
	case sf::Keyboard::Key::Num2:
		unequipSlot(Equipment::Slot::Body);
		break;
	case sf::Keyboard::Key::Num3:
		unequipSlot(Equipment::Slot::Accessory);
		break;
	default:
		break;
	}
}

void ExploreScene::useSelected()
{
	Player& p = fGame.player();
	Item* item = p.inventory().at(static_cast<size_t>(fInvIndex));
	if (item == nullptr)
	{
		return;
	}

	std::string name = item->name();                  // the item may be used up (deleted) below
	bool consumable = item->isConsumable();
	if (!item->canUse(p))
	{
		fInvMessage = "You can't use " + name + " right now";
		return;
	}
	if (p.useItem(item))
	{
		fInvMessage = (consumable ? "Used " : "Equipped ") + name;
	}
	else
	{
		fInvMessage = "Nothing happened";
	}

	int count = static_cast<int>(p.inventory().count());
	if (fInvIndex >= count)
	{
		fInvIndex = count > 0 ? count - 1 : 0;
	}
}

void ExploreScene::unequipSlot(Equipment::Slot aSlot)
{
	Player& p = fGame.player();
	const Equipment* worn = p.equipped(aSlot);
	if (worn == nullptr)
	{
		fInvMessage = "Nothing is worn there";
		return;
	}
	std::string name = worn->name();
	if (p.unequip(aSlot))
	{
		fInvMessage = "Took off " + name;
	}
	else
	{
		fInvMessage = "Your bag is full";
	}
}

void ExploreScene::drawInventory(sf::RenderWindow& aWindow) const
{
	Player& p = fGame.player();
	const sf::Color normal(230, 230, 240);
	const sf::Color picked(255, 220, 90);
	const sf::Color faded(130, 130, 150);

	sf::RectangleShape shade({ static_cast<float>(Game::WINDOW_WIDTH), static_cast<float>(Game::WINDOW_HEIGHT) });
	shade.setFillColor(sf::Color(0, 0, 0, 170));
	aWindow.draw(shade);

	sf::RectangleShape panel({ 708.f, 560.f });
	panel.setPosition({ 30.f, 30.f });
	panel.setFillColor(sf::Color(28, 28, 44, 250));
	panel.setOutlineColor(sf::Color(100, 100, 150));
	panel.setOutlineThickness(2.f);
	aWindow.draw(panel);

	// ---- left: the bag (walked with the iterator) ----
	fGame.drawText(aWindow,
		"Bag  (" + std::to_string(p.inventory().count()) + "/" + std::to_string(Inventory::CAPACITY) + ")",
		52.f, 44.f, 24, normal);

	int row = 0;
	for (ItemIterator it = p.inventory().begin(); it != p.inventory().end(); ++it)
	{
		Item* item = *it;
		float y = 88.f + static_cast<float>(row) * 40.f;
		bool isPicked = row == fInvIndex;
		if (isPicked)
		{
			sf::RectangleShape bar({ 350.f, 38.f });
			bar.setPosition({ 46.f, y - 4.f });
			bar.setFillColor(sf::Color(60, 60, 100));
			aWindow.draw(bar);
		}
		fGame.drawFrame(aWindow, Assets::Id::Items, item->iconIndex(), 0, 54.f, y, 30.f);
		fGame.drawText(aWindow, item->name(), 96.f, y + 3.f, 20,
			isPicked ? picked : (item->canUse(p) ? normal : faded));
		row++;
	}
	if (row == 0)
	{
		fGame.drawText(aWindow, "(empty)", 96.f, 91.f, 20, faded);
	}

	// ---- right: equipment, stats, skills ----
	fGame.drawText(aWindow, "Equipped", 430.f, 44.f, 24, normal);
	const char* slotNames[] = { "[1] Weapon", "[2] Body", "[3] Accessory" };
	for (size_t i = 0; i < Equipment::SLOT_COUNT; i++)
	{
		const Equipment* worn = p.equipped(static_cast<Equipment::Slot>(i));
		fGame.drawText(aWindow,
			std::string(slotNames[i]) + ":  " + (worn != nullptr ? worn->name() : std::string("-")),
			430.f, 86.f + static_cast<float>(i) * 28.f, 17, worn != nullptr ? normal : faded);
	}

	fGame.drawText(aWindow, "Stats", 430.f, 190.f, 22, normal);
	fGame.drawText(aWindow, "Attack  " + std::to_string(p.attack()) + "    Defence  " + std::to_string(p.defence()),
		430.f, 222.f, 17, normal);
	fGame.drawText(aWindow, "Health  " + std::to_string(p.health()) + "/" + std::to_string(p.maxHealth()) +
		"    Mana  " + std::to_string(p.mana()) + "/" + std::to_string(p.maxMana()),
		430.f, 248.f, 17, normal);
	fGame.drawText(aWindow, "Key fragments  " + std::to_string(p.keyFragments()) + "/" + std::to_string(Player::FRAGMENTS_NEEDED),
		430.f, 274.f, 17, sf::Color(240, 200, 70));

	fGame.drawText(aWindow, "Skills", 430.f, 320.f, 22, normal);
	for (size_t i = 0; i < p.skillCount(); i++)
	{
		Skill* skill = p.skill(i);
		fGame.drawText(aWindow, skill->name() + "   " + std::to_string(skill->manaCost()) + " MP",
			430.f, 352.f + static_cast<float>(i) * 26.f, 17, normal);
	}

	// ---- bottom: description, last message, help ----
	Item* selected = p.inventory().at(static_cast<size_t>(fInvIndex));
	if (selected != nullptr)
	{
		fGame.drawText(aWindow, selected->description(), 52.f, 480.f, 17, sf::Color(180, 200, 230));
	}
	fGame.drawText(aWindow, fInvMessage, 52.f, 510.f, 19, sf::Color(255, 230, 120));
	fGame.drawText(aWindow, "W/S: select    Enter: use or equip    1/2/3: take off    I or Esc: close",
		52.f, 556.f, 14, faded);
}
