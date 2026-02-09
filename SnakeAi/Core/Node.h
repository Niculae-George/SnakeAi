#pragma once

enum class NodeType { Empty, Snake, Food, Border };

struct Node {
    NodeType type = NodeType::Empty;
    int row = 0, col = 0;
};
