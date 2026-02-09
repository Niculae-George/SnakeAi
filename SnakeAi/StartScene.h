#pragma once

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <string>

class StartScene : public Scene {
public:
    StartScene(const sf::Vector2u& windowSize, const std::string& lastStatsText);
    void handleEvent(const sf::Event& ev) override;
    SceneAction update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
private:
    sf::Font font_;
    sf::Text title_;
    sf::Text statsLabel_;
    sf::RectangleShape startButton_;
    sf::Text startText_;
    sf::RectangleShape exitButton_;
    sf::Text exitText_;
    SceneAction action_ = SceneAction::None;
};
