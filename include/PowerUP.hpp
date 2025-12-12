#pragma once
#include <SFML/Graphics.hpp>
#include "Snake.hpp"

struct PowerUp {
    enum class Type { Magnet, Double, Phase } type = Type::Magnet;
    sf::Vector2i pos;

    void draw(sf::RenderWindow &w, int cellSize) const {
        sf::CircleShape c(cellSize/2.f - 6.f);
        c.setPosition(pos.x*cellSize + 6.f, pos.y*cellSize + 6.f);
        if (type==Type::Magnet) c.setFillColor(sf::Color::Cyan);
        if (type==Type::Double) c.setFillColor(sf::Color::Magenta);
        if (type==Type::Phase) c.setFillColor(sf::Color::Blue);
        w.draw(c);
    }

    void applyTo(Snake &s) const {
        if (type==Type::Magnet) { s.activeMagnet = true; s.magnetRemaining += sf::seconds(8); }
        if (type==Type::Double) { s.activeDouble = true; s.doubleRemaining += sf::seconds(8); }
        if (type==Type::Phase) { s.activePhase = true; s.phaseRemaining += sf::seconds(8); }
    }
};
