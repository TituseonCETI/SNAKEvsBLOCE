#pragma once
#include <SFML/Graphics.hpp>


struct Apple {
sf::Vector2i pos;
void draw(sf::RenderWindow &w, int cellSize) const {
sf::CircleShape c(cellSize/2 - 4);
c.setPosition(pos.x*cellSize + 4, pos.y*cellSize + 4);
c.setFillColor(sf::Color::Red);
w.draw(c);
}
sf::Vector2f getCenterF() const { return sf::Vector2f(pos.x + 0.5f, pos.y + 0.5f); }
void move(sf::Vector2f delta) { pos.x = int(pos.x + delta.x); pos.y = int(pos.y + delta.y); }
};