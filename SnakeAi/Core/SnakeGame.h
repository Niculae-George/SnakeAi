#pragma once
#include <vector>
#include <deque>
#include <SFML/System.hpp>
#include "Node.h"
#include "Config.h"

class SnakeGame {
public:
    SnakeGame();
    
    void reset();
    bool step(sf::Vector2i direction);
    void spawnFood();
    sf::Vector2i findBestMoveBFS() const;
    bool isPathAvailable(sf::Vector2i start, sf::Vector2i end) const;

    // Getters
    const std::vector<std::vector<Node>>& getGrid() const { return grid_; }
    const std::deque<sf::Vector2i>& getSnakeBody() const { return snakeBody_; }
    sf::Vector2i getFoodPos() const;
    int getScore() const { return (int)snakeBody_.size(); }

private:
    int rows_ = Config::GRID_ROWS;
    int cols_ = Config::GRID_COLS;
    std::vector<std::vector<Node>> grid_;
    std::deque<sf::Vector2i> snakeBody_;
};
