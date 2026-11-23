#include "BattleScene.h"
#include <cmath>
#include "Game.h"
#include "LpcSheet.h"
#include "Skill.h"

BattleScene::BattleScene(Game& aGame, Player& aPlayer, Monster* aMonsters[], size_t aCount)
	: fGame(aGame),
	fPlayer(aPlayer),
	fMonsters{},
	fMonsterCount(0),
	fBattle(nullptr),
	fPhase(Phase::Deciding),
	fTimer(0.f),
	fActor(nullptr),
	fHitIndex(-1),
	fCasting(false),
	fPlayerFlinch(false),
	fMessage("A fight begins!"),
	fMenuIndex(0),
	fSkillIndex(0),
	fTargetIndex(0),
	fPending(Pending::Attack),
	fShownPlayerHp(static_cast<float>(aPlayer.health())),
	fShownMonsterHp{}
{
	for (size_t i = 0; i < aCount && i < Battle::MAX_MONSTERS; i++)
	{
		fMonsters[fMonsterCount] = aMonsters[i];
		fShownMonsterHp[fMonsterCount] = static_cast<float>(aMonsters[i]->health());
		fMonsterCount++;
	}
	fBattle = new Battle(&fPlayer, fMonsters, fMonsterCount);
	fOrderText = fBattle->turnOrderText();
}

BattleScene::~BattleScene()
{
	delete fBattle;      // the player and the monsters belong to other objects
}

// ---------------------------------------------------------------- helpers
int BattleScene::aliveMonsterCount() const
{
	int count = 0;
	for (size_t i = 0; i < fMonsterCount; i++)
	{
		if (fMonsters[i]->isAlive())
		{
			count++;
		}
	}
	return count;
}

int BattleScene::firstAliveIndex() const
{
	for (size_t i = 0; i < fMonsterCount; i++)
	{
		if (fMonsters[i]->isAlive())
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

int BattleScene::nextAliveIndex(int aFrom, int aStep) const
{
	int n = static_cast<int>(fMonsterCount);
	int index = aFrom;
	for (int tries = 0; tries < n; tries++)
	{
		index = (index + aStep + n) % n;
		if (fMonsters[index]->isAlive())
		{
			return index;
		}
	}
	return aFrom;
}

void BattleScene::endBattle()
{
	fPlayer.clearShield();                      // shield layers only last for one fight
	if (fBattle->playerWon())
	{
		fGame.requestExplore();
	}
	else
	{
		fGame.requestGameOver();
	}
}

// ---------------------------------------------------------------- turns
void BattleScene::performTurn(const PlayerAction* aAction)
{
	int playerBefore = fPlayer.health();
	int shieldBefore = fPlayer.shieldPoints();
	int monsterBefore[Battle::MAX_MONSTERS] = {};
	for (size_t i = 0; i < fMonsterCount; i++)
	{
		monsterBefore[i] = fMonsters[i]->health();
	}

	fActor = fBattle->step(aAction);
	fHitIndex = -1;
	fCasting = false;
	fPlayerFlinch = false;

	if (fActor == nullptr)
	{
		fPhase = Phase::Deciding;       // a dead entry was skipped
		fOrderText = fBattle->turnOrderText();
		return;
	}

	if (fActor == &fPlayer)
	{
		int damage = 0;
		for (size_t i = 0; i < fMonsterCount; i++)
		{
			int lost = monsterBefore[i] - fMonsters[i]->health();
			damage += lost;
			if (lost > 0)
			{
				fHitIndex = static_cast<int>(i);
			}
		}
		int healed = fPlayer.health() - playerBefore;
		int shieldGain = fPlayer.shieldPoints() - shieldBefore;

		fCasting = aAction != nullptr && aAction->kind == PlayerAction::Kind::UseSkill && aAction->skill != nullptr;
		if (fCasting)
		{
			const std::string& skillName = aAction->skill->name();
			if (damage > 0)
			{
				fMessage = fPlayer.name() + " casts " + skillName + " for " + std::to_string(damage);
			}
			else if (healed > 0)
			{
				fMessage = fPlayer.name() + " casts " + skillName + ": +" + std::to_string(healed) + " health";
			}
			else if (shieldGain > 0)
			{
				fMessage = fPlayer.name() + " casts " + skillName + ": +" + std::to_string(shieldGain) + " shield";
			}
			else
			{
				fMessage = fPlayer.name() + " casts " + skillName;
			}
		}
		else
		{
			fMessage = fPlayer.name() + " hits for " + std::to_string(damage);
		}
	}
	else
	{
		int hpLost = playerBefore - fPlayer.health();
		int absorbed = shieldBefore - fPlayer.shieldPoints();
		fPlayerFlinch = hpLost > 0;

		if (hpLost > 0 && absorbed > 0)
		{
			fMessage = fActor->name() + " hits you for " + std::to_string(hpLost) +
				" (shield soaked " + std::to_string(absorbed) + ")";
		}
		else if (hpLost > 0)
		{
			fMessage = fActor->name() + " hits you for " + std::to_string(hpLost);
		}
		else if (absorbed > 0)
		{
			fMessage = "Your shield soaks " + std::to_string(absorbed) + " from " + fActor->name();
		}
		else
		{
			fMessage = fActor->name() + " attacks";
		}
	}

	fOrderText = fBattle->turnOrderText();
	fPhase = Phase::Acting;
	fTimer = 0.f;
}

void BattleScene::commit(Monster* aTarget)
{
	PlayerAction action{ PlayerAction::Kind::Attack, nullptr, aTarget };
	if (fPending == Pending::Skill)
	{
		action.kind = PlayerAction::Kind::UseSkill;
		action.skill = fPlayer.skill(static_cast<size_t>(fSkillIndex));
	}
	performTurn(&action);
}

void BattleScene::beginAttack()
{
	fPending = Pending::Attack;
	if (aliveMonsterCount() <= 1)
	{
		int index = firstAliveIndex();
		commit(index >= 0 ? fMonsters[index] : nullptr);
		return;
	}
	fTargetIndex = firstAliveIndex();
	fPhase = Phase::ChoosingTarget;
	fMessage = "Pick a target (W/S, Enter).  Esc: back";
}

void BattleScene::chooseSkill()
{
	Skill* skill = fPlayer.skill(static_cast<size_t>(fSkillIndex));
	if (skill == nullptr)
	{
		return;
	}
	if (!skill->canUse(fPlayer))
	{
		fMessage = "You can't use " + skill->name() + " right now";
		return;
	}

	fPending = Pending::Skill;
	if (skill->needsTarget() && aliveMonsterCount() > 1)
	{
		fTargetIndex = firstAliveIndex();
		fPhase = Phase::ChoosingTarget;
		fMessage = "Pick a target (W/S, Enter).  Esc: back";
		return;
	}

	Monster* target = nullptr;
	if (skill->needsTarget())
	{
		int index = firstAliveIndex();
		target = index >= 0 ? fMonsters[index] : nullptr;
	}
	commit(target);
}

// ---------------------------------------------------------------- input
void BattleScene::handleInput(const sf::Event& aEvent)
{
	if (fGame.hasPendingRequest())
	{
		return;
	}
	const auto* key = aEvent.getIf<sf::Event::KeyPressed>();
	if (key == nullptr)
	{
		return;
	}

	bool confirm = key->code == sf::Keyboard::Key::Space || key->code == sf::Keyboard::Key::Enter;
	bool up = key->code == sf::Keyboard::Key::W || key->code == sf::Keyboard::Key::Up;
	bool down = key->code == sf::Keyboard::Key::S || key->code == sf::Keyboard::Key::Down;
	bool back = key->code == sf::Keyboard::Key::Escape || key->code == sf::Keyboard::Key::Backspace;

	switch (fPhase)
	{
	case Phase::ChoosingAction:
		if (up || down)
		{
			fMenuIndex = 1 - fMenuIndex;
		}
		else if (confirm)
		{
			if (fMenuIndex == 0)
			{
				beginAttack();
			}
			else if (fPlayer.skillCount() == 0)
			{
				fMessage = "You don't know any skills yet";
			}
			else
			{
				fPhase = Phase::ChoosingSkill;
				fSkillIndex = 0;
				fMessage = "Pick a skill (W/S, Enter).  Esc: back";
			}
		}
		break;

	case Phase::ChoosingSkill:
	{
		int n = static_cast<int>(fPlayer.skillCount());
		if (up && n > 0)
		{
			fSkillIndex = (fSkillIndex + n - 1) % n;
		}
		else if (down && n > 0)
		{
			fSkillIndex = (fSkillIndex + 1) % n;
		}
		else if (back)
		{
			fPhase = Phase::ChoosingAction;
			fMessage = "Your turn";
		}
		else if (confirm)
		{
			chooseSkill();
		}
		break;
	}

	case Phase::ChoosingTarget:
		if (up)
		{
			fTargetIndex = nextAliveIndex(fTargetIndex, -1);
		}
		else if (down)
		{
			fTargetIndex = nextAliveIndex(fTargetIndex, 1);
		}
		else if (back)
		{
			fPhase = fPending == Pending::Attack ? Phase::ChoosingAction : Phase::ChoosingSkill;
			fMessage = "Your turn";
		}
		else if (confirm)
		{
			commit(fMonsters[fTargetIndex]);
		}
		break;

	case Phase::Finished:
		if (confirm)
		{
			endBattle();
		}
		break;

	default:
		break;
	}
}

// ---------------------------------------------------------------- update
float BattleScene::easeToward(float aShown, int aActual, float aDt) const
{
	const float speed = 30.f;       // HP per second
	float target = static_cast<float>(aActual);
	if (aShown > target)
	{
		aShown -= speed * aDt;
		if (aShown < target)
		{
			aShown = target;
		}
	}
	else if (aShown < target)
	{
		aShown += speed * aDt;
		if (aShown > target)
		{
			aShown = target;
		}
	}
	return aShown;
}

void BattleScene::easeBars(float aDt)
{
	fShownPlayerHp = easeToward(fShownPlayerHp, fPlayer.health(), aDt);
	for (size_t i = 0; i < fMonsterCount; i++)
	{
		fShownMonsterHp[i] = easeToward(fShownMonsterHp[i], fMonsters[i]->health(), aDt);
	}
}

void BattleScene::update(float aDt)
{
	easeBars(aDt);

	switch (fPhase)
	{
	case Phase::Deciding:
	{
		if (fBattle->isOver())
		{
			fPhase = Phase::Finished;
			fMessage = fBattle->playerWon() ? "Victory! Press Enter" : "You were defeated. Press Enter";
			break;
		}
		Combatant* next = fBattle->peekNext();
		if (next == nullptr)
		{
			fPhase = Phase::Finished;
			break;
		}
		if (!next->isAlive())
		{
			fBattle->step();                         // throws the dead entry away
			fOrderText = fBattle->turnOrderText();
		}
		else if (next == &fPlayer)
		{
			fPhase = Phase::ChoosingAction;
			fMenuIndex = 0;
			fMessage = "Your turn";
		}
		else
		{
			fPhase = Phase::MonsterPause;
			fTimer = 0.f;
		}
		break;
	}

	case Phase::ChoosingAction:
	case Phase::ChoosingSkill:
	case Phase::ChoosingTarget:
		break;      // handleInput() drives these

	case Phase::MonsterPause:
		fTimer += aDt;
		if (fTimer >= MONSTER_PAUSE)
		{
			performTurn(nullptr);
		}
		break;

	case Phase::Acting:
		fTimer += aDt;
		if (fTimer >= ACT_TIME)
		{
			fPhase = Phase::Deciding;
		}
		break;

	case Phase::Finished:
		break;
	}
}

// ---------------------------------------------------------------- drawing
void BattleScene::drawBar(sf::RenderWindow& aWindow, float aX, float aY, float aShown, int aMax) const
{
	const float width = 100.f;

	sf::RectangleShape back({ width, 10.f });
	back.setPosition({ aX, aY });
	back.setFillColor(sf::Color(90, 20, 20));
	aWindow.draw(back);

	float ratio = aMax > 0 ? aShown / static_cast<float>(aMax) : 0.f;
	if (ratio < 0.f)
	{
		ratio = 0.f;
	}
	if (ratio > 1.f)
	{
		ratio = 1.f;
	}
	sf::RectangleShape front({ width * ratio, 10.f });
	front.setPosition({ aX, aY });
	front.setFillColor(sf::Color(60, 200, 80));
	aWindow.draw(front);
}

// One blue outline per shield layer, the newest (top) layer is the outermost.
void BattleScene::drawShield(sf::RenderWindow& aWindow, float aX, float aY, float aSize) const
{
	int layers = fPlayer.shieldLayers();
	for (int k = 0; k < layers; k++)
	{
		float w = aSize * 0.40f + 12.f * static_cast<float>(k);
		float h = aSize * 0.72f + 12.f * static_cast<float>(k);
		sf::RectangleShape ring({ w, h });
		ring.setOrigin({ w / 2.f, h / 2.f });
		ring.setPosition({ aX + aSize / 2.f, aY + aSize * 0.62f });
		ring.setFillColor(sf::Color(80, 150, 255, 25));
		ring.setOutlineColor(sf::Color(110, 180, 255, 210));
		ring.setOutlineThickness(2.f);
		aWindow.draw(ring);
	}
}

void BattleScene::drawMenu(sf::RenderWindow& aWindow) const
{
	if (fPhase != Phase::ChoosingAction && fPhase != Phase::ChoosingSkill && fPhase != Phase::ChoosingTarget)
	{
		return;
	}

	const float x = 20.f;
	const float y = 468.f;
	sf::RectangleShape panel({ 340.f, 128.f });
	panel.setPosition({ x, y });
	panel.setFillColor(sf::Color(28, 28, 44, 235));
	panel.setOutlineColor(sf::Color(100, 100, 150));
	panel.setOutlineThickness(2.f);
	aWindow.draw(panel);

	const sf::Color normal(230, 230, 240);
	const sf::Color picked(255, 220, 90);
	const sf::Color disabled(110, 110, 125);

	if (fPhase == Phase::ChoosingAction)
	{
		fGame.drawText(aWindow, std::string(fMenuIndex == 0 ? "> " : "   ") + "Attack",
			x + 14.f, y + 10.f, 22, fMenuIndex == 0 ? picked : normal);
		fGame.drawText(aWindow, std::string(fMenuIndex == 1 ? "> " : "   ") + "Skill",
			x + 14.f, y + 42.f, 22, fMenuIndex == 1 ? picked : normal);
		fGame.drawText(aWindow, "W/S: choose    Enter: confirm", x + 14.f, y + 100.f, 14, disabled);
	}
	else if (fPhase == Phase::ChoosingSkill)
	{
		for (size_t i = 0; i < fPlayer.skillCount(); i++)
		{
			Skill* skill = fPlayer.skill(i);
			bool usable = skill->canUse(fPlayer);
			bool isPicked = static_cast<int>(i) == fSkillIndex;
			sf::Color color = !usable ? disabled : (isPicked ? picked : normal);
			fGame.drawText(aWindow,
				std::string(isPicked ? "> " : "   ") + skill->name() + "   " + std::to_string(skill->manaCost()) + " MP",
				x + 14.f, y + 6.f + static_cast<float>(i) * 26.f, 20, color);
		}
		Skill* shown = fPlayer.skill(static_cast<size_t>(fSkillIndex));
		if (shown != nullptr)
		{
			fGame.drawText(aWindow, shown->description(), x + 14.f, y + 100.f, 14, sf::Color(180, 200, 230));
		}
	}
	else
	{
		fGame.drawText(aWindow, "Choose a target", x + 14.f, y + 10.f, 22, picked);
		fGame.drawText(aWindow, "W/S: change    Enter: confirm    Esc: back", x + 14.f, y + 100.f, 14, disabled);
	}
}

void BattleScene::draw(sf::RenderWindow& aWindow)
{
	const float pi = 3.14159265f;
	const sf::Color deadTint(70, 70, 80);
	const float size = 128.f;           // a 64x64 frame drawn twice as big

	float progress = 0.f;
	if (fPhase == Phase::Acting)
	{
		progress = fTimer / ACT_TIME;
		if (progress > 0.999f)
		{
			progress = 0.999f;
		}
	}
	float lunge = std::sin(pi * progress) * 60.f;       // forward and back (monsters)

	sf::RectangleShape fallback({ size, size });

	// ---- Player (left): idle, spear thrust / spell cast, flinch when hurt ----
	float px = 96.f;
	float py = 260.f;

	int heroRow = Lpc::WALK_ROW + Lpc::Right;           // standing, facing the monsters
	int heroCol = 0;
	bool playerActing = fPhase == Phase::Acting && fActor == &fPlayer;
	bool playerHit = fPhase == Phase::Acting && fActor != nullptr && fActor != &fPlayer && fPlayerFlinch;
	if (playerActing && fCasting)
	{
		heroRow = Lpc::SPELLCAST_ROW + Lpc::Right;
		heroCol = static_cast<int>(progress * Lpc::SPELLCAST_FRAMES);
	}
	else if (playerActing)
	{
		heroRow = Lpc::THRUST_ROW + Lpc::Right;
		heroCol = static_cast<int>(progress * Lpc::THRUST_FRAMES);
	}
	else if (playerHit)
	{
		// The LPC "hurt" row is really a fall to the ground: use only the first
		// frames for a normal hit, and play the whole fall when the blow was fatal.
		heroRow = Lpc::HURT_ROW;
		int frames = fPlayer.isAlive() ? 2 : Lpc::HURT_FRAMES;
		heroCol = static_cast<int>(progress * frames);
	}
	else if (!fPlayer.isAlive())
	{
		heroRow = Lpc::HURT_ROW;
		heroCol = Lpc::HURT_FRAMES - 1;                 // lying down
	}

	if (!fGame.drawFrame(aWindow, Assets::Id::Hero, heroCol, heroRow, px, py, size))
	{
		fallback.setPosition({ px, py });
		fallback.setFillColor(fPlayer.isAlive() ? sf::Color(70, 130, 230) : deadTint);
		aWindow.draw(fallback);
	}
	drawShield(aWindow, px, py, size);
	drawBar(aWindow, px + 14.f, py + 6.f, fShownPlayerHp, fPlayer.maxHealth());
	fGame.drawText(aWindow, fPlayer.name(), px + 10.f, py + 132.f, 18, sf::Color::White);
	fGame.drawText(aWindow,
		"HP " + std::to_string(fPlayer.health()) + "/" + std::to_string(fPlayer.maxHealth()) +
		"  MP " + std::to_string(fPlayer.mana()) + "/" + std::to_string(fPlayer.maxMana()),
		px + 10.f, py + 156.f, 16, sf::Color(200, 200, 210));
	if (fPlayer.shieldLayers() > 0)
	{
		fGame.drawText(aWindow,
			"Shield x" + std::to_string(fPlayer.shieldLayers()) + "  (" + std::to_string(fPlayer.shieldPoints()) + " pts)",
			px + 10.f, py + 178.f, 16, sf::Color(130, 190, 255));
	}

	// ---- Monsters (right, stacked): idle, slash when attacking, hurt when hit ----
	for (size_t i = 0; i < fMonsterCount; i++)
	{
		float mx = 520.f;
		float my = 40.f + static_cast<float>(i) * 150.f;
		bool alive = fMonsters[i]->isAlive();
		bool acting = fPhase == Phase::Acting && fActor == fMonsters[i];
		bool beingHit = fPhase == Phase::Acting && fHitIndex == static_cast<int>(i);

		int row = Lpc::WALK_ROW + Lpc::Left;           // standing, facing the hero
		int col = 0;
		if (acting)
		{
			row = Lpc::SLASH_ROW + Lpc::Left;
			col = static_cast<int>(progress * Lpc::SLASH_FRAMES);
			mx -= lunge * 0.5f;
		}
		else if (beingHit)
		{
			row = Lpc::HURT_ROW;
			int frames = alive ? 2 : Lpc::HURT_FRAMES;       // flinch, or the full fall if it died
			col = static_cast<int>(progress * frames);
		}
		else if (!alive)
		{
			row = Lpc::HURT_ROW;
			col = Lpc::HURT_FRAMES - 1;                // collapsed
		}

		if (!fGame.drawFrame(aWindow, Assets::Id::Skeleton, col, row, mx, my, size))
		{
			fallback.setPosition({ mx, my });
			fallback.setFillColor(alive ? sf::Color(200, 120, 40) : deadTint);
			aWindow.draw(fallback);
		}
		drawBar(aWindow, 520.f + 14.f, my + 6.f, fShownMonsterHp[i], fMonsters[i]->maxHealth());
		fGame.drawText(aWindow,
			fMonsters[i]->name() + "  " + std::to_string(fMonsters[i]->health()) + "/" +
			std::to_string(fMonsters[i]->maxHealth()),
			520.f + 10.f, my + 128.f, 16, sf::Color::White);

		// arrow beside the monster that is about to be targeted
		if (fPhase == Phase::ChoosingTarget && fTargetIndex == static_cast<int>(i))
		{
			fGame.drawText(aWindow, ">", 488.f, my + 44.f, 40, sf::Color(255, 220, 90));
		}
	}

	// ---- Menu and text ----
	drawMenu(aWindow);
	fGame.drawText(aWindow, "Turn order: " + fOrderText, 20.f, 16.f, 18, sf::Color(220, 220, 120));
	fGame.drawText(aWindow, fMessage, 20.f, 604.f, 24, sf::Color::White);
}
