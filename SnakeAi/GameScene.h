#pragma once

#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>
#include "Core/AiAgent.h"
#include "Core/SnakeGame.h"
#include "Core/Config.h"

class GameScene : public Scene {
public:
    GameScene(const sf::Vector2u& windowSize);
    void handleEvent(const sf::Event& ev) override;
    SceneAction update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    
    void onDestroy() override;
    std::string getStats() const override;

private:
    void resetGame();

    SnakeGame game_;
    AiAgent aiAgent_;
    
    int rows_;
    int cols_;
    float cellSize_;
    float offsetX_;
    float offsetY_;
    float rightMargin_;

    sf::Vector2i direction_;
    sf::Time moveInterval_;
    sf::Time timeSinceLastMove_;
    bool isGameOver_ = false;
    bool returnToMenu_ = false;

    int highScore_ = 0;
    int attempt_ = 1;

    sf::Font uiFont_;
    sf::RectangleShape statsBg_;
    sf::Text statsTitle_;
    sf::Text lengthText_;
    sf::Text highScoreText_;
    sf::Text attemptText_;

    std::deque<Experience> memory_;
};
