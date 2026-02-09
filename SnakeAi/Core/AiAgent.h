#pragma once
#include <SFML/System.hpp>
#include <deque>
#include <vector>
#include "Node.h" 
#include "SimpleNN.h"

struct Experience {
    std::vector<double> state;
    int action;
    double reward;
    std::vector<double> nextState;
    bool done;
};

class AiAgent {
public:
    AiAgent();
    
    // Core Functions
    int getAction(const std::vector<double>& state);
    void train(const std::vector<Experience>& batch);
    void decayEpsilon();
    
    // Helpers
    std::vector<double> getState(const std::vector<std::vector<Node>>& grid, const sf::Vector2i& head, const sf::Vector2i& tail, const sf::Vector2i& food, const sf::Vector2i& direction);
    
    // IO
    void save(const std::string& filename);
    void load(const std::string& filename);

    NeuralNetwork brain;
    double epsilon = 0.5; // Exploration rate
    double gamma = 0.9;   // Discount factor
    
private:
    int inputSize = 34; // 24 (rays) + 2 (food) + 2 (tail) + 3 (danger) + 3 (flood fill)
    int hiddenSize = 32;
    int outputSize = 3; // Straight, Left, Right
};