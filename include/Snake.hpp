#pragma once
#include <SFML/Graphics.hpp>
#include <deque>


class Snake {
public:
Snake(int cellSize = 24);
void reset(const sf::Vector2i &start);
void handleEvent(const sf::Event &e);
void update(sf::Time dt);
void draw(sf::RenderWindow &w, int cellSize);


bool checkAppleCollision(const sf::Vector2i &applePos) const;
bool checkCollision(const sf::Vector2i &pos) const;
bool withinBounds(int w, int h) const;
void wrapPosition(int w, int h);
bool checkSelfCollision() const;
sf::Vector2f getHeadPositionF() const;


void grow(int segments);
void increaseSpeed(float delta);


// Active powerups
bool activeMagnet = false;
bool activeDouble = false;
bool activePhase = false;
sf::Time magnetRemaining = sf::Time::Zero;
sf::Time doubleRemaining = sf::Time::Zero;
sf::Time phaseRemaining = sf::Time::Zero;


private:
std::deque<sf::Vector2i> body;
sf::Vector2i dir;
float speed; // cells per second
int cellSize;
float accu;
};