#include "Game.h"
#include <optional>
#include "ExploreScene.h"
#include "BattleScene.h"
#include "EndScene.h"
#include "Skill.h"

namespace
{
	// The hero starts with one skill; the others are found as scrolls in the rooms.
	Player* makeHero()
	{
		Player* hero = new Player("Hero", "The player", 30, 6, 2, 10);
		hero->learnSkill(new Fireball());
		return hero;
	}
}

Game::Game()
	: fWindow(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Dungeon"),
	fFontLoaded(false),
	fPlayer(makeHero()),
	fExplore(nullptr),
	fBattle(nullptr),
	fEnd(nullptr),
	fCurrent(nullptr),
	fRequest(Request::None),
	fPendingMonsters{},
	fPendingCount(0)
{
	fWindow.setFramerateLimit(60);

	// Try a font from the assets folder first, then Windows Arial.
	fFontLoaded = fFont.openFromFile("assets/font.ttf") || fFont.openFromFile("C:/Windows/Fonts/arial.ttf");
	fAssets.loadAll();

	fExplore = new ExploreScene(*this);
	fCurrent = fExplore;
}

Game::~Game()
{
	delete fBattle;
	delete fEnd;
	delete fExplore;
	delete fPlayer;
}

void Game::run()
{
	sf::Clock clock;

	while (fWindow.isOpen())
	{
		float dt = clock.restart().asSeconds();
		if (dt > 0.1f)
		{
			dt = 0.1f;      // avoid huge jumps after a pause
		}

		while (const std::optional event = fWindow.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				fWindow.close();
			}
			else
			{
				fCurrent->handleInput(*event);
			}
		}
		if (!fWindow.isOpen())
		{
			break;
		}

		fCurrent->update(dt);

		fWindow.clear(sf::Color(20, 20, 28));
		fCurrent->draw(fWindow);
		fWindow.display();

		applyRequest();
	}
}

void Game::applyRequest()
{
	switch (fRequest)
	{
	case Request::None:
		return;

	case Request::StartBattle:
		delete fBattle;
		fBattle = new BattleScene(*this, *fPlayer, fPendingMonsters, fPendingCount);
		fCurrent = fBattle;
		break;

	case Request::BackToExplore:
		delete fBattle;
		fBattle = nullptr;
		fExplore->removeDefeated();
		fCurrent = fExplore;
		break;

	case Request::ShowGameOver:
		delete fBattle;
		fBattle = nullptr;
		delete fEnd;
		fEnd = new EndScene(*this, false);
		fCurrent = fEnd;
		break;

	case Request::ShowVictory:
		delete fEnd;
		fEnd = nullptr;
		fEnd = new EndScene(*this, true);
		fCurrent = fEnd;
		break;

	case Request::RestartGame:
		delete fBattle;
		fBattle = nullptr;
		delete fEnd;
		fEnd = nullptr;
		delete fExplore;
		delete fPlayer;
		fPlayer = makeHero();
		fExplore = new ExploreScene(*this);
		fCurrent = fExplore;
		break;
	}
	fRequest = Request::None;
}

void Game::requestBattle(Monster* aMonsters[], size_t aCount)
{
	fPendingCount = 0;
	for (size_t i = 0; i < aCount && i < Battle::MAX_MONSTERS; i++)
	{
		fPendingMonsters[fPendingCount++] = aMonsters[i];
	}
	fRequest = Request::StartBattle;
}

void Game::requestExplore()  { fRequest = Request::BackToExplore; }
void Game::requestGameOver() { fRequest = Request::ShowGameOver; }
void Game::requestVictory()  { fRequest = Request::ShowVictory; }
void Game::requestRestart()  { fRequest = Request::RestartGame; }

bool Game::hasPendingRequest() const
{
	return fRequest != Request::None;
}

void Game::quit()
{
	fWindow.close();
}

Player& Game::player()
{
	return *fPlayer;
}

void Game::drawText(sf::RenderTarget& aTarget, const std::string& aText,
	float aX, float aY, unsigned aSize, sf::Color aColor) const
{
	if (!fFontLoaded)
	{
		return;     // no font found: skip text instead of crashing
	}
	sf::Text text(fFont, aText, aSize);
	text.setFillColor(aColor);
	text.setPosition({ aX, aY });
	aTarget.draw(text);
}

bool Game::drawSprite(sf::RenderTarget& aTarget, Assets::Id aId,
	float aX, float aY, float aWidth, float aHeight, sf::Color aTint, bool aFlipX) const
{
	if (!fAssets.has(aId))
	{
		return false;
	}

	const sf::Texture& texture = fAssets.get(aId);
	sf::Vector2u size = texture.getSize();
	float scaleX = aWidth / static_cast<float>(size.x);
	float scaleY = aHeight / static_cast<float>(size.y);

	sf::Sprite sprite(texture);
	sprite.setColor(aTint);
	if (aFlipX)
	{
		sprite.setScale({ -scaleX, scaleY });     // mirror left-right
		sprite.setPosition({ aX + aWidth, aY });
	}
	else
	{
		sprite.setScale({ scaleX, scaleY });
		sprite.setPosition({ aX, aY });
	}
	aTarget.draw(sprite);
	return true;
}

bool Game::drawFrame(sf::RenderTarget& aTarget, Assets::Id aId, int aCol, int aRow,
	float aX, float aY, float aSize, sf::Color aTint, bool aFlipX) const
{
	if (!fAssets.has(aId))
	{
		return false;
	}
	int frame = fAssets.frameSize(aId);
	if (frame <= 0)
	{
		return false;
	}

	const sf::Texture& texture = fAssets.get(aId);
	sf::Vector2u size = texture.getSize();
	if ((aCol + 1) * frame > static_cast<int>(size.x) || (aRow + 1) * frame > static_cast<int>(size.y))
	{
		return false;       // frame outside the sheet
	}

	sf::Sprite sprite(texture);
	sprite.setTextureRect(sf::IntRect({ aCol * frame, aRow * frame }, { frame, frame }));
	sprite.setColor(aTint);
	float scale = aSize / static_cast<float>(frame);
	if (aFlipX)
	{
		sprite.setScale({ -scale, scale });
		sprite.setPosition({ aX + aSize, aY });
	}
	else
	{
		sprite.setScale({ scale, scale });
		sprite.setPosition({ aX, aY });
	}
	aTarget.draw(sprite);
	return true;
}
