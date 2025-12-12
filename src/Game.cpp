#include "Game.hpp"
window.draw(timeText);
window.draw(powerupTimersText);


if (state == State::Paused) {
sf::Text t("PAUSA - P para reanudar, R reiniciar, ESC menu", font, 28);
t.setPosition(winW/2 - 300, winH/2 - 20);
window.draw(t);
}


if (state == State::GameOver) {
sf::Text t("GAME OVER\nPress R to restart or ESC to menu", font, 30);
t.setPosition(winW/2 - 280, winH/2 - 40);
window.draw(t);
sf::Text stats((std::string)"Tiempo: " + std::to_string(int(gameClock.getElapsedTime().asSeconds())) + "s\nManzanas: " + std::to_string(applesEaten), font, 20);
stats.setPosition(winW/2 - 200, winH/2 + 60);
window.draw(stats);
}
}
window.display();
}


void Game::spawnApple()
{
std::uniform_int_distribution<int> dx(1, winW / cellSize - 2);
std::uniform_int_distribution<int> dy(1, winH / cellSize - 2);
Apple a; a.pos = sf::Vector2i(dx(rng), dy(rng));
apples.push_back(a);
}


void Game::spawnPowerUp()
{
std::uniform_int_distribution<int> dx(1, winW / cellSize - 2);
std::uniform_int_distribution<int> dy(1, winH / cellSize - 2);
PowerUp p;
int k = rng()%3;
if (k==0) p.type = PowerUp::Type::Magnet;
if (k==1) p.type = PowerUp::Type::Double;
if (k==2) p.type = PowerUp::Type::Phase;
p.pos = sf::Vector2i(dx(rng), dy(rng));
powerups.push_back(p);
}


void Game::spawnBlocks()
{
std::uniform_int_distribution<int> dx(1, winW / cellSize - 3);
std::uniform_int_distribution<int> dy(1, winH / cellSize - 3);
std::uniform_int_distribution<int> size(1,3);
for (int i=0;i<maxBlocks;i++) {
Block b; b.pos = sf::Vector2i(dx(rng), dy(rng));
b.size = size(rng);
blocks.push_back(b);
}
}