#include "StartScene.h"
#include "EmbeddedAssets.h"
#include <iostream>

StartScene::StartScene(const sf::Vector2u& windowSize, const std::string& lastStatsText)
{
    // Load bundled font from memory
    if (!font_.loadFromMemory(GameFont_ttf, GameFont_ttf_len)) {
        std::cerr << "Failed to load embedded font" << std::endl;
        // Handle error if needed, though with embedded assets this should rarely fail
    }

    title_.setFont(font_);
    title_.setString("Snake AI");
    title_.setCharacterSize(48);
    title_.setFillColor(sf::Color::White);
    title_.setPosition(20.f, 20.f);
    statsLabel_.setFont(font_);
    statsLabel_.setString(lastStatsText);
    statsLabel_.setCharacterSize(20);
    statsLabel_.setFillColor(sf::Color::White);
    statsLabel_.setPosition(20.f, 90.f);

    // Buttons
    const float btnW = windowSize.x * 0.25f;
    const float btnH = 50.f;

    startButton_.setSize({btnW, btnH});
    startButton_.setFillColor(sf::Color(80, 160, 80));
    startButton_.setPosition(windowSize.x * 0.5f - btnW - 10.f, windowSize.y * 0.5f);

    startText_.setFont(font_);
    startText_.setString("Start Game");
    startText_.setCharacterSize(24);
    startText_.setFillColor(sf::Color::Black);
    // center text inside button
    startText_.setPosition(startButton_.getPosition().x + 10.f, startButton_.getPosition().y + 8.f);

    exitButton_.setSize({btnW, btnH});
    exitButton_.setFillColor(sf::Color(200, 80, 80));
    exitButton_.setPosition(windowSize.x * 0.5f + 10.f, windowSize.y * 0.5f);

    exitText_.setFont(font_);
    exitText_.setString("Exit");
    exitText_.setCharacterSize(24);
    exitText_.setFillColor(sf::Color::Black);
    exitText_.setPosition(exitButton_.getPosition().x + 10.f, exitButton_.getPosition().y + 8.f);
}

void StartScene::handleEvent(const sf::Event& ev)
{
    if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left) {
        const sf::Vector2f mousePos(static_cast<float>(ev.mouseButton.x), static_cast<float>(ev.mouseButton.y));
        if (startButton_.getGlobalBounds().contains(mousePos)) {
            action_ = SceneAction::StartGame;
        } else if (exitButton_.getGlobalBounds().contains(mousePos)) {
            action_ = SceneAction::Exit;
        }
    }
}

SceneAction StartScene::update(sf::Time dt)
{
    // No animation for now
    SceneAction a = action_;
    action_ = SceneAction::None;
    return a;
}

void StartScene::draw(sf::RenderWindow& window)
{
    window.draw(title_);
    window.draw(statsLabel_);
    window.draw(startButton_);
    window.draw(startText_);
    window.draw(exitButton_);
    window.draw(exitText_);
}
