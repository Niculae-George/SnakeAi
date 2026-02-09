#include "GameScene.h"
#include "EmbeddedAssets.h"
#include <iostream>
#include <algorithm>

GameScene::GameScene(const sf::Vector2u &windowSize)
{
    const float winW = static_cast<float>(windowSize.x);
    const float winH = static_cast<float>(windowSize.y);
    
    rows_ = Config::GRID_ROWS;
    cols_ = Config::GRID_COLS;
    cellSize_ = winH / (rows_ + 1.0f);
    
    float totalGridW = cellSize_ * cols_;
    float totalGridH = cellSize_ * rows_;
    offsetY_ = cellSize_ * 0.5f;
    offsetX_ = offsetY_;
    rightMargin_ = winW - offsetX_ - totalGridW;

    if (!uiFont_.loadFromMemory(GameFont_ttf, GameFont_ttf_len)) {
        std::cerr << "Failed to load embedded font" << std::endl;
    }

    const float statsX = offsetX_ + totalGridW + 10.f;
    const float statsY = offsetY_ + 10.f;
    
    statsBg_.setSize({std::max(0.f, rightMargin_ - 20.f), std::max(0.f, totalGridH - 20.f)});
    statsBg_.setPosition(statsX, statsY);
    statsBg_.setFillColor(sf::Color(40, 40, 40));

    statsTitle_.setFont(uiFont_);
    statsTitle_.setCharacterSize(20);
    statsTitle_.setString("AI Evaluation");
    statsTitle_.setPosition(statsX + 8.f, statsY + 8.f);

    lengthText_.setFont(uiFont_);
    lengthText_.setCharacterSize(16);
    lengthText_.setPosition(statsX + 8.f, statsY + 40.f);

    highScoreText_.setFont(uiFont_);
    highScoreText_.setCharacterSize(16);
    highScoreText_.setFillColor(sf::Color::Yellow);
    highScoreText_.setPosition(statsX + 8.f, statsY + 70.f);

    attemptText_.setFont(uiFont_);
    attemptText_.setCharacterSize(16);
    attemptText_.setFillColor(sf::Color::Cyan);
    attemptText_.setPosition(statsX + 8.f, statsY + 100.f);

    aiAgent_.load("model.txt");
    aiAgent_.epsilon = 0.01; // Minimal exploration for evaluation mode
    
    resetGame();
}

void GameScene::resetGame() {
    game_.reset();
    direction_ = {1, 0};
    moveInterval_ = Config::MOVE_INTERVAL;
    timeSinceLastMove_ = sf::Time::Zero;
    isGameOver_ = false;
}

void GameScene::handleEvent(const sf::Event &ev) {
    if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape) {
        returnToMenu_ = true;
    }
}

SceneAction GameScene::update(sf::Time dt)
{
    if (returnToMenu_) return SceneAction::ReturnToMenu;
    
    if (isGameOver_) {
        if (game_.getScore() > highScore_) highScore_ = game_.getScore();
        aiAgent_.decayEpsilon();
        std::cout << "Attempt: " << attempt_++ << " | Score: " << game_.getScore() << " | Epsilon: " << aiAgent_.epsilon << std::endl;
        resetGame();
        return SceneAction::None;
    }

    timeSinceLastMove_ += dt;
    static int stepCounter = 0;
    
    while (timeSinceLastMove_ >= moveInterval_)
    {
        timeSinceLastMove_ -= moveInterval_;
        stepCounter++;

        sf::Vector2i head = game_.getSnakeBody().front();
        sf::Vector2i tail = game_.getSnakeBody().back();
        sf::Vector2i food = game_.getFoodPos();
        
        std::vector<double> state = aiAgent_.getState(game_.getGrid(), head, tail, food, direction_);

        int action = aiAgent_.getAction(state);
        sf::Vector2i moveDir;
        
        // Relative to absolute direction mapping
        if (action == 1)      moveDir = {direction_.y, -direction_.x}; // Left
        else if (action == 2) moveDir = {-direction_.y, direction_.x}; // Right
        else                  moveDir = direction_;                    // Straight

        direction_ = moveDir;
        bool alive = game_.step(direction_);
        sf::Vector2i nextHead = game_.getSnakeBody().front();
        
        double reward = Config::REWARD_STEP;
        if (!alive) {
            reward = Config::REWARD_DEATH;
            isGameOver_ = true;
        } else if (nextHead == food) {
            reward = Config::REWARD_FOOD;
        }

        std::vector<double> nextState = aiAgent_.getState(game_.getGrid(), nextHead, game_.getSnakeBody().back(), food, direction_);
        memory_.push_back({state, action, reward, nextState, isGameOver_});
        
        if (memory_.size() > Config::REPLAY_MEMORY_SIZE) memory_.pop_front();

        // Train periodically from memory
        if (stepCounter % 5 == 0) {
            std::vector<Experience> batch;
            if (memory_.size() > Config::BATCH_SIZE) {
                for (int i = 0; i < Config::BATCH_SIZE; ++i) batch.push_back(memory_[rand() % memory_.size()]);
            } else {
                batch.assign(memory_.begin(), memory_.end());
            }
            aiAgent_.train(batch);
        }
        
        if (isGameOver_) break;
    }
    
    return SceneAction::None;
}

void GameScene::draw(sf::RenderWindow &window)
{
    const float winW = static_cast<float>(window.getSize().x);
    const float winH = static_cast<float>(window.getSize().y);
    
    // Draw Frame
    sf::RectangleShape frame;
    frame.setFillColor(Config::COLOR_FRAME);
    frame.setSize({winW, offsetY_}); frame.setPosition(0, 0); window.draw(frame);
    frame.setSize({winW, offsetY_}); frame.setPosition(0, winH - offsetY_); window.draw(frame);
    frame.setSize({offsetX_, winH}); frame.setPosition(0, 0); window.draw(frame);
    frame.setSize({rightMargin_, winH}); frame.setPosition(winW - rightMargin_, 0); window.draw(frame);

    // Update Stats Text
    lengthText_.setString("Score: " + std::to_string(game_.getScore()));
    highScoreText_.setString("High Score: " + std::to_string(highScore_));
    attemptText_.setString("Attempt: " + std::to_string(attempt_));
    
    window.draw(statsBg_);
    window.draw(statsTitle_);
    window.draw(lengthText_);
    window.draw(highScoreText_);
    // Note: attempt_ logic was a bit messy, simplified here.

    // Draw Grid
    const auto &grid = game_.getGrid();
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            const float x = offsetX_ + c * cellSize_;
            const float y = offsetY_ + r * cellSize_;
            
            sf::RectangleShape cell({cellSize_ - Config::CELL_PADDING, cellSize_ - Config::CELL_PADDING});
            cell.setPosition(x + Config::CELL_PADDING * 0.5f, y + Config::CELL_PADDING * 0.5f);
            
            if (grid[r][c].type == NodeType::Snake) {
                cell.setFillColor((game_.getSnakeBody().front() == sf::Vector2i(c, r)) ? Config::COLOR_SNAKE_HEAD : Config::COLOR_SNAKE_BODY);
            } else if (grid[r][c].type == NodeType::Food) {
                cell.setFillColor(Config::COLOR_FOOD);
            } else {
                cell.setFillColor(Config::COLOR_BG);
            }
            window.draw(cell);

            // Draw direction indicator on head
            if (game_.getSnakeBody().front() == sf::Vector2i(c, r)) {
                sf::CircleShape dot(cellSize_ * 0.2f);
                dot.setFillColor(sf::Color::Black);
                dot.setOrigin(dot.getRadius(), dot.getRadius());
                dot.setPosition(x + cellSize_ * 0.5f + direction_.x * (cellSize_ * 0.25f), 
                               y + cellSize_ * 0.5f + direction_.y * (cellSize_ * 0.25f));
                window.draw(dot);
            }
        }
    }
}

void GameScene::onDestroy() {
    aiAgent_.save("model.txt");
}

std::string GameScene::getStats() const {
    return "High Score: " + std::to_string(highScore_);
}
