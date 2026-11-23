#include "EndScene.h"
#include "Game.h"

EndScene::EndScene(Game& aGame, bool aVictory)
	: fGame(aGame), fVictory(aVictory)
{
}

void EndScene::handleInput(const sf::Event& aEvent)
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
	if (key->code == sf::Keyboard::Key::Enter)
	{
		fGame.requestRestart();
	}
	else if (key->code == sf::Keyboard::Key::Escape)
	{
		fGame.quit();
	}
}

void EndScene::update(float /*aDt*/)
{
}

void EndScene::draw(sf::RenderWindow& aWindow)
{
	if (fVictory)
	{
		fGame.drawText(aWindow, "YOU ESCAPED!", 190.f, 200.f, 56, sf::Color(90, 220, 110));
	}
	else
	{
		fGame.drawText(aWindow, "YOU DIED", 250.f, 200.f, 56, sf::Color(230, 80, 80));
	}
	fGame.drawText(aWindow, "Enter: play again     Esc: quit", 200.f, 310.f, 24, sf::Color::White);
}
