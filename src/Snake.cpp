#include "Snake.hpp"
#include <algorithm>

Snake::Snake(int csize): cellSize(csize) { reset({5,5}); }

void Snake::reset(const sf::Vector2i &start)
{
    body.clear();
    body.push_back(start);
    body.push_back(start + sf::Vector2i(-1,0));
    body.push_back(start + sf::Vector2i(-2,0));
    dir = {1,0}; speed = 6.0f; accu = 0.f;
    activeMagnet = activeDouble = activePhase = false;
    magnetRemaining = doubleRemaining = phaseRemaining = sf::Time::Zero;
}

void Snake::handleEvent(const sf::Event &e)
{
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Up && dir.y==0) dir = {0,-1};
        if (e.key.code == sf::Keyboard::Down && dir.y==0) dir = {0,1};
        if (e.key.code == sf::Keyboard::Left && dir.x==0) dir = {-1,0};
        if (e.key.code == sf::Keyboard::Right && dir.x==0) dir = {1,0};
    }
}

void Snake::update(sf::Time dt)
{
    float step = speed * dt.asSeconds();
    accu += step;
    while (accu >= 1.0f) {
        sf::Vector2i newHead = body.front() + dir;
        body.push_front(newHead);
        body.pop_back();
        accu -= 1.0f;
    }

    if (activeMagnet) {
        magnetRemaining -= dt;
        if (magnetRemaining <= sf::Time::Zero) activeMagnet = false;
    }
    if (activeDouble) {
        doubleRemaining -= dt;
        if (doubleRemaining <= sf::Time::Zero) activeDouble = false;
    }
    if (activePhase) {
        phaseRemaining -= dt;
        if (phaseRemaining <= sf::Time::Zero) activePhase = false;
    }
}

void Snake::draw(sf::RenderWindow &w, int csize)
{
    sf::RectangleShape r(sf::Vector2f(csize-2, csize-2));
    for (size_t i=0;i<body.size();++i) {
        r.setPosition(body[i].x * csize + 1.f, body[i].y * csize + 1.f);
        if (i==0) r.setFillColor(sf::Color::Yellow);
        else r.setFillColor(sf::Color::Green);
        w.draw(r);
    }
}

bool Snake::checkAppleCollision(const sf::Vector2i &applePos) const
{
    return body.front() == applePos;
}

bool Snake::checkCollision(const sf::Vector2i &pos) const
{
    for (auto &s : body) if (s == pos) return true;
    return false;
}

bool Snake::withinBounds(int w, int h) const
{
    auto hpos = body.front();
    return hpos.x >= 0 && hpos.x < w && hpos.y >= 0 && hpos.y < h;
}

void Snake::wrapPosition(int w, int h)
{
    auto &hpos = body.front();
    if (hpos.x < 0) hpos.x = w-1;
    if (hpos.x >= w) hpos.x = 0;
    if (hpos.y < 0) hpos.y = h-1;
    if (hpos.y >= h) hpos.y = 0;
}

bool Snake::checkSelfCollision() const
{
    auto head = body.front();
    for (size_t i=1;i<body.size();++i) if (body[i]==head) return true;
    return false;
}

sf::Vector2f Snake::getHeadPositionF() const
{
    return sf::Vector2f(body.front().x * cellSize + cellSize/2.f, body.front().y * cellSize + cellSize/2.f);
}

void Snake::grow(int segments)
{
    for (int i=0;i<segments;i++) body.push_back(body.back());
}

void Snake::increaseSpeed(float delta)
{
    speed += delta / 10.f;
}
