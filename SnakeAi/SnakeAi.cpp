#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <string>
#include "Scene.h"
#include "StartScene.h"

enum class NodeType { Empty, Snake, Food, Border };

struct Node {
    NodeType type = NodeType::Empty;
    int row, col;
};

int main()
{
    const int initialWindowWidth = 1000;
    const int initialWindowHeight = 800;

    const int rows = 25;
    const int cols = 25;
    const float outlineThickness = 1.0f; 
    const float cellInnerPadding = 1.0f;

    sf::RenderWindow window(sf::VideoMode(initialWindowWidth, initialWindowHeight), "Snake AI", sf::Style::Titlebar | sf::Style::Close);

    const float winW = static_cast<float>(initialWindowWidth);
    const float winH = static_cast<float>(initialWindowHeight);

    const float idealCellH = winH / (rows + 1.0f);
    const float cellSize = idealCellH;

    const float totalGridW = cellSize * cols;
    const float totalGridH = cellSize * rows;
    const float offsetY = cellSize * 0.5f;
    const float offsetX = offsetY;
    // Right margin is whatever remains and will be used for stats panel
    const float rightMargin = winW - offsetX - totalGridW;

    std::vector<std::vector<Node>> grid(rows, std::vector<Node>(cols));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            grid[r][c] = {NodeType::Empty, r, c};
        }
    }

    const int startRow = rows / 2;
    const int startCol = cols / 2;
    grid[startRow][startCol].type = NodeType::Snake;

    // Load bundled font (assumed present in repository)
    sf::Font uiFont;
    uiFont.loadFromFile("assets/fonts/Orbitron-Regular.ttf");

    const float statsPadding = 10.f;
    const float statsX = offsetX + totalGridW + statsPadding;
    const float statsY = offsetY + statsPadding;
    const float statsW = std::max(0.f, rightMargin - statsPadding * 2.f);
    const float statsH = std::max(0.f, totalGridH - statsPadding * 2.f);

    sf::RectangleShape statsBg(sf::Vector2f(statsW, statsH));
    statsBg.setPosition(statsX, statsY);
    statsBg.setFillColor(sf::Color(40, 40, 40));

    sf::Text statsTitle;
    statsTitle.setFont(uiFont);
    statsTitle.setCharacterSize(20);
    statsTitle.setString("Stats");
    statsTitle.setFillColor(sf::Color::White);
    statsTitle.setPosition(statsX + 8.f, statsY + 8.f);

    sf::Text lengthText;
    lengthText.setFont(uiFont);
    lengthText.setCharacterSize(16);
    lengthText.setString("Length: 0");
    lengthText.setFillColor(sf::Color::White);
    lengthText.setPosition(statsX + 8.f, statsY + 40.f);

    // Start scene
    StartScene startScene(window.getSize(), "Previous: none");
    bool startChosen = false;

    while (window.isOpen() && !startChosen)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
            startScene.handleEvent(event);
        }

        if (!window.isOpen()) break;

        SceneAction act = startScene.update(sf::Time::Zero);
        if (act == SceneAction::Exit) {
            window.close();
            break;
        } else if (act == SceneAction::StartGame) {
            startChosen = true;
            break;
        }

        window.clear(sf::Color::Black);
        startScene.draw(window);
        window.display();
    }

    // If start chosen, run the game loop (simple grid view for now)
    if (window.isOpen() && startChosen) {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            window.clear(sf::Color::Black);

            const sf::Color frameColor(100, 100, 100);
            sf::RectangleShape topBorder(sf::Vector2f(winW, offsetY));
            topBorder.setPosition(0.f, 0.f);
            topBorder.setFillColor(frameColor);

            sf::RectangleShape bottomBorder(sf::Vector2f(winW, offsetY));
            bottomBorder.setPosition(0.f, winH - offsetY);
            bottomBorder.setFillColor(frameColor);

            sf::RectangleShape leftBorder(sf::Vector2f(offsetX, winH));
            leftBorder.setPosition(0.f, 0.f);
            leftBorder.setFillColor(frameColor);

            sf::RectangleShape rightBorder(sf::Vector2f(rightMargin, winH));
            rightBorder.setPosition(winW - rightMargin, 0.f);
            rightBorder.setFillColor(frameColor);

            window.draw(topBorder);
            window.draw(bottomBorder);
            window.draw(leftBorder);
            window.draw(rightBorder);

            // Update stats (snake length) and draw stats panel
            int snakeLength = 0;
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    if (grid[r][c].type == NodeType::Snake) ++snakeLength;
                }
            }
            lengthText.setString(std::string("Length: ") + std::to_string(snakeLength));

            window.draw(statsBg);
            window.draw(statsTitle);
            window.draw(lengthText);

            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    const float x = offsetX + c * cellSize;
                    const float y = offsetY + r * cellSize;

                    // Cell background
                    sf::RectangleShape cellRect(sf::Vector2f(cellSize - cellInnerPadding, cellSize - cellInnerPadding));
                    cellRect.setPosition(x + cellInnerPadding * 0.5f, y + cellInnerPadding * 0.5f);
                    cellRect.setFillColor(grid[r][c].type == NodeType::Snake ? sf::Color::Green : sf::Color::Black);
                    window.draw(cellRect);

                    // Cell outline
                    sf::RectangleShape outline(sf::Vector2f(cellSize - cellInnerPadding, cellSize - cellInnerPadding));
                    outline.setPosition(x + cellInnerPadding * 0.5f, y + cellInnerPadding * 0.5f);
                    outline.setFillColor(sf::Color::Transparent);
                    outline.setOutlineThickness(outlineThickness);
                    outline.setOutlineColor(sf::Color(60, 60, 60));
                    window.draw(outline);
                }
            }

            window.display();
        }
    }

    return 0;
}
