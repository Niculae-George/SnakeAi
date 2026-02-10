#pragma once
#ifndef HEADLESS_BUILD
#include <SFML/Graphics.hpp>
#else
#include <SFML/System.hpp>
#endif

namespace Config {
    // Window Settings
    inline const int WINDOW_WIDTH = 1000;
    inline const int WINDOW_HEIGHT = 800;
    inline const char* WINDOW_TITLE = "Snake AI - Reinforcement Learning";

    // Grid Settings
    inline const int GRID_ROWS = 25;
    inline const int GRID_COLS = 25;
    inline const float OUTLINE_THICKNESS = 1.0f;
    inline const float CELL_PADDING = 1.0f;

    // AI Training Settings
    inline const sf::Time MOVE_INTERVAL = sf::milliseconds(100);
    inline const int REPLAY_MEMORY_SIZE = 10000;
    inline const int BATCH_SIZE = 32;
    inline const double GAMMA = 0.99; // Value future rewards more
    inline const double EPSILON_DECAY = 0.997; // Faster decay (hits 0.01 in ~1500 games)
    inline const double MIN_EPSILON = 0.00001;

    // Rewards
    inline const double REWARD_FOOD = 20.0;
    inline const double REWARD_DEATH = -50.0;  // Reduced from -100 (recoverable)
    inline const double REWARD_STEP = -0.05;   // Reduced from -0.5 (encourage movement)
    inline const double REWARD_CLOSER = 0.5;   // Positive reinforcement
    inline const double REWARD_AWAY = -0.6;    // Gentle correction

#ifndef HEADLESS_BUILD
    // Colors
    inline const sf::Color COLOR_SNAKE_HEAD = sf::Color(0, 255, 0);
    inline const sf::Color COLOR_SNAKE_BODY = sf::Color(0, 200, 0);
    inline const sf::Color COLOR_FOOD = sf::Color::Red;
    inline const sf::Color COLOR_GRID_LINES = sf::Color(60, 60, 60);
    inline const sf::Color COLOR_BG = sf::Color::Black;
    inline const sf::Color COLOR_FRAME = sf::Color(100, 100, 100);
#endif
}
