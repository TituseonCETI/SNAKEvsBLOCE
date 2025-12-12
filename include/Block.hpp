#pragma once
#include <SFML/Graphics.hpp>

struct Block {
    sf::Vector2i pos;
    int size = 1; // 1..3 cells

    void draw(sf::RenderWindow &w, int cellSize) const {
        sf::RectangleShape r(sf::Vector2f(cellSize*size - 2.f, cellSize*size - 2.f));
        r.setPosition(pos.x*cellSize + 1.f, pos.y*cellSize + 1.f);
        r.setFillColor(sf::Color(120,120,120));
        w.draw(r);
    }
};
