#pragma once
#include <SFML/Graphics.hpp>

class Button {
public:
    Button() {}
    Button(sf::Vector2f pos, float w, float h, const std::string &text, const sf::Font &font) {
        rect.setSize({w,h});
        rect.setPosition(pos);
        rect.setFillColor(sf::Color(50,50,50,200));
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(2.f);

        label.setFont(font);
        label.setString(text);
        label.setCharacterSize(24);
        sf::FloatRect tb = label.getLocalBounds();
        label.setPosition(pos.x + (w - tb.width)/2.f - tb.left, pos.y + (h - tb.height)/2.f - tb.top);
    }

    bool contains(const sf::Vector2f &p) const { return rect.getGlobalBounds().contains(p); }
    void draw(sf::RenderWindow &w) const { w.draw(rect); w.draw(label); }

private:
    sf::RectangleShape rect;
    sf::Text label;
};
