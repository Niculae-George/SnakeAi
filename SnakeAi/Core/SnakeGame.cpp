#include "SnakeGame.h"
#include <random>
#include <queue>

SnakeGame::SnakeGame() {
    reset();
}

void SnakeGame::reset() {
    grid_.assign(rows_, std::vector<Node>(cols_));
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            grid_[r][c] = {NodeType::Empty, r, c};
        }
    }
    snakeBody_.clear();
    const int startRow = rows_ / 2;
    const int startCol = cols_ / 2;
    snakeBody_.push_front({startCol, startRow});
    grid_[startRow][startCol].type = NodeType::Snake;
    spawnFood();
}

void SnakeGame::spawnFood() {
    std::vector<sf::Vector2i> emptyCells;
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (grid_[r][c].type == NodeType::Empty) emptyCells.push_back({c, r});
        }
    }
    if (!emptyCells.empty()) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, (int)emptyCells.size() - 1);
        sf::Vector2i pos = emptyCells[dis(gen)];
        grid_[pos.y][pos.x].type = NodeType::Food;
    }
}

sf::Vector2i SnakeGame::getFoodPos() const {
    for(int r=0; r<rows_; ++r) {
        for(int c=0; c<cols_; ++c) {
            if (grid_[r][c].type == NodeType::Food) return {c, r};
        }
    }
    return {-1, -1};
}

bool SnakeGame::step(sf::Vector2i direction) {
    sf::Vector2i head = snakeBody_.front();
    sf::Vector2i newHead = head + direction;

    // Boundary check
    if (newHead.x < 0 || newHead.x >= cols_ || newHead.y < 0 || newHead.y >= rows_) return false;

    NodeType nextType = grid_[newHead.y][newHead.x].type;
    
    // Collision check
    if (nextType == NodeType::Snake && newHead != snakeBody_.back()) return false;

    if (nextType == NodeType::Food) {
        snakeBody_.push_front(newHead);
        grid_[newHead.y][newHead.x].type = NodeType::Snake;
        spawnFood();
    } else {
        sf::Vector2i tail = snakeBody_.back();
        grid_[tail.y][tail.x].type = NodeType::Empty;
        snakeBody_.pop_back();
        snakeBody_.push_front(newHead);
        grid_[newHead.y][newHead.x].type = NodeType::Snake;
    }
    return true;
}

bool SnakeGame::isPathAvailable(sf::Vector2i start, sf::Vector2i end) const {
    if (start == end) return true;
    std::queue<sf::Vector2i> q;
    q.push(start);
    std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
    visited[start.y][start.x] = true;

    sf::Vector2i dirs[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    while(!q.empty()) {
        sf::Vector2i curr = q.front(); q.pop();
        if (curr == end) return true;
        for(const auto& d : dirs) {
            sf::Vector2i next = curr + d;
            if (next.x >= 0 && next.x < cols_ && next.y >= 0 && next.y < rows_) {
                if (!visited[next.y][next.x] && (grid_[next.y][next.x].type != NodeType::Snake || next == snakeBody_.back())) {
                    visited[next.y][next.x] = true;
                    q.push(next);
                }
            }
        }
    }
    return false;
}

sf::Vector2i SnakeGame::findBestMoveBFS() const {
    sf::Vector2i head = snakeBody_.front();
    sf::Vector2i foodPos = getFoodPos();
    if (foodPos.x == -1) return {0, 0};

    std::queue<sf::Vector2i> q; q.push(head);
    std::vector<std::vector<sf::Vector2i>> parent(rows_, std::vector<sf::Vector2i>(cols_, {-1, -1}));
    std::vector<std::vector<bool>> visited(rows_, std::vector<bool>(cols_, false));
    visited[head.y][head.x] = true;

    sf::Vector2i dirs[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    bool found = false;
    while(!q.empty()) {
        sf::Vector2i curr = q.front(); q.pop();
        if (curr == foodPos) { found = true; break; }
        for(const auto& d : dirs) {
            sf::Vector2i next = curr + d;
            if (next.x >= 0 && next.x < cols_ && next.y >= 0 && next.y < rows_) {
                if (!visited[next.y][next.x] && (grid_[next.y][next.x].type != NodeType::Snake || next == snakeBody_.back())) {
                    visited[next.y][next.x] = true;
                    parent[next.y][next.x] = curr;
                    q.push(next);
                }
            }
        }
    }

    if (found) {
        sf::Vector2i curr = foodPos;
        while (parent[curr.y][curr.x] != head) curr = parent[curr.y][curr.x];
        sf::Vector2i firstMove = curr - head;

        // --- SAFETY CHECK ---
        // If I take this move and reach the food, can I still reach my tail?
        // We simulate the head at foodPos and the body length increased by 1.
        // For simplicity, we just check if foodPos has a path to the current tail.
        if (isPathAvailable(foodPos, snakeBody_.back())) {
            return firstMove;
        }
    }

    // No safe path to food found, return zero to let AI decide (or fallback)
    return {0, 0};
}
