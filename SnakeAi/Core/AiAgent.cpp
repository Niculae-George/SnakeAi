#include "AiAgent.h"
#include "Config.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>

AiAgent::AiAgent() {
    brain.addLayer(inputSize);
    brain.addLayer(hiddenSize);
    brain.addLayer(outputSize);
    epsilon = 1.0;
}

int AiAgent::getAction(const std::vector<double>& state) {
    if (((double)rand() / RAND_MAX) < epsilon) {
        return rand() % outputSize;
    }
    std::vector<double> outputs = brain.feedForward(state);
    return (int)std::distance(outputs.begin(), std::max_element(outputs.begin(), outputs.end()));
}

void AiAgent::train(const std::vector<Experience>& batch) {
    if (batch.empty()) return;

    for (const auto& exp : batch) {
        std::vector<double> currentQs = brain.feedForward(exp.state);
        std::vector<double> nextQs = brain.feedForward(exp.nextState);
        double maxNextQ = *std::max_element(nextQs.begin(), nextQs.end());
        
        double targetQ = exp.reward;
        if (!exp.done) {
            targetQ += gamma * maxNextQ;
        }
        
        std::vector<double> targets = currentQs;
        targets[exp.action] = targetQ;
        
        brain.feedForward(exp.state); 
        brain.backPropagate(targets);
    }
}

void AiAgent::decayEpsilon() {
    if (epsilon > Config::MIN_EPSILON) {
        epsilon *= Config::EPSILON_DECAY;
    }
}

std::vector<double> AiAgent::getState(const std::vector<std::vector<Node>>& grid, 
                                     const sf::Vector2i& head, 
                                     const sf::Vector2i& tail, 
                                     const sf::Vector2i& food, 
                                     const sf::Vector2i& direction) 
{
    std::vector<double> state;
    const int rows = (int)grid.size();
    const int cols = (int)grid[0].size();
    
    // 1. Raycasting (8 directions)
    sf::Vector2i dirs[8] = {
        {0, -1}, {1, -1}, {1, 0}, {1, 1},
        {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
    };

    for (int i = 0; i < 8; ++i) {
        sf::Vector2i d = dirs[i];
        sf::Vector2i p = head;
        double distWall = 0, distFood = 0, distBody = 0, distance = 0;
        bool foundFood = false, foundBody = false;

        while (true) {
            p += d;
            distance += 1.0;
            
            if (p.x < 0 || p.x >= cols || p.y < 0 || p.y >= rows) {
                distWall = 1.0 / distance;
                break;
            }
            if (!foundFood && p == food) {
                distFood = 1.0 / distance;
                foundFood = true;
            }
            if (!foundBody && grid[p.y][p.x].type == NodeType::Snake) {
                distBody = 1.0 / distance;
                foundBody = true;
            }
        }
        state.push_back(distWall);
        state.push_back(distFood);
        state.push_back(distBody);
    }

    // 2. Relative Food Vector
    double dx = (double)(food.x - head.x) / cols;
    double dy = (double)(food.y - head.y) / rows;
    state.push_back(dx * direction.x + dy * direction.y); // Forward
    state.push_back(dx * direction.y - dy * direction.x); // Side

    // 3. Relative Tail Vector (Helps snake follow its own tail)
    double tx = (double)(tail.x - head.x) / cols;
    double ty = (double)(tail.y - head.y) / rows;
    state.push_back(tx * direction.x + ty * direction.y);
    state.push_back(tx * direction.y - ty * direction.x);

    // 4. Survival Sensors (Immediate Danger)
    auto isDanger = [&](sf::Vector2i p) {
        if (p.x < 0 || p.x >= cols || p.y < 0 || p.y >= rows) return 1.0;
        if (grid[p.y][p.x].type == NodeType::Snake && p != tail) return 1.0;
        return 0.0;
    };

    sf::Vector2i df = direction;
    sf::Vector2i dl = {direction.y, -direction.x};
    sf::Vector2i dr = {-direction.y, direction.x};

    state.push_back(isDanger(head + df));
    state.push_back(isDanger(head + dl));
    state.push_back(isDanger(head + dr));

    // 5. Flood Fill Accessibility
    auto getAccessibility = [&](sf::Vector2i startPos) {
        if (isDanger(startPos)) return 0.0;
        
        int count = 0;
        std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
        std::queue<sf::Vector2i> q;
        
        q.push(startPos);
        visited[startPos.y][startPos.x] = true;
        count++;

        sf::Vector2i moves[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        while (!q.empty()) {
            sf::Vector2i curr = q.front();
            q.pop();
            for (auto& m : moves) {
                sf::Vector2i next = curr + m;
                if (next.x >= 0 && next.x < cols && next.y >= 0 && next.y < rows && 
                    !visited[next.y][next.x] && 
                    (grid[next.y][next.x].type != NodeType::Snake || next == tail)) 
                {
                    visited[next.y][next.x] = true;
                    count++;
                    q.push(next);
                }
            }
        }
        return (double)count / (rows * cols);
    };

    state.push_back(getAccessibility(head + df));
    state.push_back(getAccessibility(head + dl));
    state.push_back(getAccessibility(head + dr));

    return state;
}

void AiAgent::save(const std::string& filename) {
    std::string tempFilename = filename + ".tmp";
    std::ofstream ofs(tempFilename);
    if (!ofs.is_open()) {
        std::cerr << "Error: Could not save model to " << tempFilename << std::endl;
        return;
    }

    ofs << "VER2\n" << epsilon << "\n";
    for (const auto& layer : brain.layers) {
        if (layer.prevSize == 0) continue;
        for (double b : layer.biases) ofs << b << " ";
        ofs << "\n";
        for (const auto& row : layer.weights) {
            for (double w : row) ofs << w << " ";
            ofs << "\n";
        }
    }
    ofs.close();

    // Atomic swap: This prevents other pods from reading a half-written file
    if (std::rename(tempFilename.c_str(), filename.c_str()) != 0) {
        std::cerr << "Error: Atomic rename failed for " << filename << std::endl;
    }
}

void AiAgent::load(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cout << "No model found at " << filename << ". Starting fresh." << std::endl;
        return;
    }

    std::string header;
    ifs >> header;
    if (header == "VER2") {
        ifs >> epsilon;
    } else {
        std::cout << "Old format detected. Resetting for compatibility." << std::endl;
        epsilon = 0.5;
        ifs.close();
        return;
    }

    for (auto& layer : brain.layers) {
        if (layer.prevSize == 0) continue;
        for (int i = 0; i < layer.size; ++i) ifs >> layer.biases[i];
        for (int i = 0; i < layer.size; ++i) {
            for (int j = 0; j < layer.prevSize; ++j) ifs >> layer.weights[i][j];
        }
    }
    ifs.close();
    std::cout << "Model loaded successfully. Epsilon: " << epsilon << std::endl;
}