#pragma once
#include <SFML/Graphics.hpp>

struct Apple {
    sf::Vector2i pos; // grid position

    void draw(sf::RenderWindow &w, int cellSize) const {
        sf::CircleShape c(cellSize/2.f - 4.f);
        c.setPosition(pos.x*cellSize + 4.f, pos.y*cellSize + 4.f);
        c.setFillColor(sf::Color::Red);
        w.draw(c);
    }
};
