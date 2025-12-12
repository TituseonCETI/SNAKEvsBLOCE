#pragma once
#include <SFML/Graphics.hpp>


struct Block {
sf::Vector2i pos;
int size = 1; // 1..3 blocks (square)
void draw(sf::RenderWindow &w, int cellSize) const {
sf::RectangleShape r(sf::Vector2f(cellSize*size - 2, cellSize*size - 2));
r.setPosition(pos.x*cellSize + 1, pos.y*cellSize + 1);
r.setFillColor(sf::Color(120,120,120));
w.draw(r);
}
};