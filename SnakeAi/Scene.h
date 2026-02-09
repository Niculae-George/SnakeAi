#pragma once

#include <SFML/Graphics.hpp>

enum class SceneAction { None, StartGame, Exit };

class Scene {
public:
    virtual ~Scene() = default;
    virtual void handleEvent(const sf::Event& ev) = 0;

    virtual SceneAction update(sf::Time dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};
