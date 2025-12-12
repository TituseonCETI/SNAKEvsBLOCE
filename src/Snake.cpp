#include "Snake.hpp"
for (size_t i=0;i<body.size();++i) {
r.setPosition(body[i].x * csize + 1, body[i].y * csize + 1);
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
// any segment same as pos
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
speed += delta/10.f; // tune
}