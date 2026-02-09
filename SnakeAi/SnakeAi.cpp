#ifndef HEADLESS_BUILD
#include <SFML/Graphics.hpp>
#endif
#include <memory>
#include <ctime>
#include <string>
#include <iostream>
#include "Scene.h"
#include "StartScene.h"
#include "GameScene.h"
#include "HeadlessTrainer.h"
#include "Core/Config.h"

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned>(time(NULL)));

    // Check for headless flag
    if (argc > 1 && std::string(argv[1]) == "--headless") {
        int attempts = 1000;
        std::string loadFile = "model.txt";
        std::string saveFile = "model.txt";

        if (argc > 2) attempts = std::stoi(argv[2]);
        if (argc > 3) loadFile = argv[3];
        if (argc > 4) saveFile = argv[4];

        HeadlessTrainer trainer(attempts, loadFile, saveFile);
        trainer.run();
        return 0;
    }

#ifdef HEADLESS_BUILD
    std::cout << "This binary was built in HEADLESS mode. Please use --headless flag." << std::endl;
    return 1;
#else
    sf::RenderWindow window(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), Config::WINDOW_TITLE, sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Initial Scene
    std::unique_ptr<Scene> currentScene = std::make_unique<StartScene>(window.getSize(), "Previous: none");

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                if (currentScene) currentScene->onDestroy();
                window.close();
                break;
            }
            if (currentScene) currentScene->handleEvent(event);
        }

        if (!window.isOpen()) break;

        sf::Time dt = clock.restart();

        if (currentScene) {
            SceneAction action = currentScene->update(dt);
            
            if (action == SceneAction::Exit) {
                currentScene->onDestroy();
                window.close();
            }
            else if (action == SceneAction::StartGame) {
                currentScene = std::make_unique<GameScene>(window.getSize());
            }
            else if (action == SceneAction::ReturnToMenu) {
                std::string stats = currentScene->getStats();
                currentScene = std::make_unique<StartScene>(window.getSize(), stats.empty() ? "No previous stats" : stats);
            }
        }

        window.clear(sf::Color::Black);
        if (currentScene) {
            currentScene->draw(window);
        }
        window.display();
    }

    return 0;
#endif
}
