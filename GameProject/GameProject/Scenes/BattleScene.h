#pragma once
#include <string>
#include "Scene.h"
#include "Battle.h"

class Game;

// Turn-based fight screen. The turn order comes from the Battle class (your Queue).
// On the player's turn a small menu offers: Attack, or Skill (then pick the skill and a target).
class BattleScene : public Scene
{
private:
	enum class Phase { Deciding, ChoosingAction, ChoosingSkill, ChoosingItem, ChoosingTarget, MonsterPause, Acting, Finished };
	enum class Pending { Attack, Skill };       // what the target picker is choosing a target for

	static constexpr float ACT_TIME = 0.5f;       // length of an attack animation
	static constexpr float MONSTER_PAUSE = 0.6f;  // pause before a monster attacks

	Game&     fGame;
	Player&   fPlayer;
	Monster*  fMonsters[Battle::MAX_MONSTERS];
	size_t    fMonsterCount;
	Battle*   fBattle;

	Phase       fPhase;
	float       fTimer;
	Combatant*  fActor;
	int         fHitIndex;        // which monster the player just hit (-1 = none)
	bool        fCasting;         // the player's last move was a skill (cast animation, not the spear)
	bool        fPlayerFlinch;    // the last monster attack really hurt the hero
	std::string fMessage;
	std::string fOrderText;

	int     fMenuIndex;           // 0 = Attack, 1 = Skill, 2 = Item
	int     fSkillIndex;
	int     fTargetIndex;
	Pending fPending;
	int         fItemIndex;                          // which potion is highlighted
	Item*       fPotions[Inventory::CAPACITY];       // the potions in the bag (collected when the item menu opens)
	size_t      fPotionCount;
	std::string fItemName;                           // name of the item just used (it may be deleted by then)

	float fShownPlayerHp;                         // HP bars ease toward the real HP
	float fShownMonsterHp[Battle::MAX_MONSTERS];

	void  performTurn(const PlayerAction* aAction);
	void  commit(Monster* aTarget);
	void  beginAttack();
	void  chooseSkill();
	void  collectPotions();
	void  commitItem(Item* aItem);
	int   aliveMonsterCount() const;
	int   firstAliveIndex() const;
	int   nextAliveIndex(int aFrom, int aStep) const;
	void  endBattle();
	void  easeBars(float aDt);
	float easeToward(float aShown, int aActual, float aDt) const;
	void  drawBar(sf::RenderWindow& aWindow, float aX, float aY, float aShown, int aMax) const;
	void  drawMenu(sf::RenderWindow& aWindow) const;
	void  drawShield(sf::RenderWindow& aWindow, float aX, float aY, float aSize) const;

public:
	BattleScene(Game& aGame, Player& aPlayer, Monster* aMonsters[], size_t aCount);
	~BattleScene() override;
	BattleScene(const BattleScene&) = delete;
	BattleScene& operator=(const BattleScene&) = delete;

	void handleInput(const sf::Event& aEvent) override;
	void update(float aDt) override;
	void draw(sf::RenderWindow& aWindow) override;
};
