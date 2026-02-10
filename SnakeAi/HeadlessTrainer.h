#pragma once
#include "Core/SnakeGame.h"
#include "Core/AiAgent.h"
#include "Core/Config.h"
#include <iostream>
#include <vector>
#include <deque>

/**
 * @brief HeadlessTrainer handles the AI training process without a GUI.
 * It supports synchronized parallel training via shared file system.
 */
class HeadlessTrainer {
public:
    HeadlessTrainer(int maxAttempts = 1000, 
                    std::string loadFile = "model.txt", 
                    std::string saveFile = "model.txt") 
        : maxAttempts_(maxAttempts), loadFile_(loadFile), saveFile_(saveFile) {}

    void run() {
        std::cout << "--- Starting Synchronized Headless Training ---" << std::endl;
        
        for (int attempt = 1; attempt <= maxAttempts_; ++attempt) {
            // 1. Sync: Load the latest global brain from other pods
            aiAgent_.load(loadFile_);

            game_.reset();
            sf::Vector2i direction = {1, 0};
            bool isGameOver = false;
            int steps = 0;

            while (!isGameOver && steps < 10000) {
                sf::Vector2i head = game_.getSnakeBody().front();
                sf::Vector2i tail = game_.getSnakeBody().back();
                sf::Vector2i food = game_.getFoodPos();
                
                std::vector<double> state = aiAgent_.getState(game_.getGrid(), head, tail, food, direction);
                
                // --- BFS Teacher Logic ---
                sf::Vector2i moveDir = game_.findBestMoveBFS();
                int action = 0;
                
                if (moveDir != sf::Vector2i(0, 0)) {
                    // Teacher found a path, force the action to follow it
                    if (moveDir == sf::Vector2i(direction.y, -direction.x)) action = 1;      // Left
                    else if (moveDir == sf::Vector2i(-direction.y, direction.x)) action = 2; // Right
                    else action = 0;                                                         // Straight
                } else {
                    // No BFS path, let the AI guess (Survival Mode)
                    action = aiAgent_.getAction(state);
                    if (action == 1)      moveDir = {direction.y, -direction.x};
                    else if (action == 2) moveDir = {-direction.y, direction.x};
                    else                  moveDir = direction;
                }

                bool alive = game_.step(moveDir);
                sf::Vector2i nextHead = game_.getSnakeBody().front();
                
                // Reward Logic
                float dPre = (float)(std::abs(head.x - food.x) + std::abs(head.y - food.y));
                float dPost = (float)(std::abs(nextHead.x - food.x) + std::abs(nextHead.y - food.y));

                double reward = Config::REWARD_STEP;
                if (!alive) {
                    reward = Config::REWARD_DEATH;
                    isGameOver = true;
                } else if (nextHead == food) {
                    reward = Config::REWARD_FOOD;
                } else {
                    reward += (dPost < dPre) ? Config::REWARD_CLOSER : Config::REWARD_AWAY;
                }

                std::vector<double> nextState = aiAgent_.getState(game_.getGrid(), nextHead, game_.getSnakeBody().back(), food, moveDir);
                memory_.push_back({state, action, reward, nextState, isGameOver});
                
                if (memory_.size() > Config::REPLAY_MEMORY_SIZE) {
                    memory_.pop_front();
                }

                if (steps % 5 == 0) {
                    trainFromMemory();
                }

                direction = moveDir;
                steps++;
            }

            // 2. Post-game cleanup and decay
            aiAgent_.decayEpsilon();
            
            // 3. Share: Save my findings back to the global brain
            if (attempt % 10 == 0) {
                aiAgent_.save(saveFile_);
            }

            // Output EVERY attempt
            std::cout << "Attempt: " << attempt 
                      << " | Score: " << game_.getScore() 
                      << " | Epsilon: " << aiAgent_.epsilon << std::endl;
        }
        
        std::cout << "--- Training Complete ---" << std::endl;
    }

private:
    void trainFromMemory() {
        std::vector<Experience> batch;
        if (memory_.size() > Config::BATCH_SIZE) {
            for (int i = 0; i < Config::BATCH_SIZE; ++i) {
                batch.push_back(memory_[rand() % memory_.size()]);
            }
        } else {
            batch.assign(memory_.begin(), memory_.end());
        }
        aiAgent_.train(batch);
    }

    SnakeGame game_;
    AiAgent aiAgent_;
    std::deque<Experience> memory_;
    int maxAttempts_;
    std::string loadFile_;
    std::string saveFile_;
};