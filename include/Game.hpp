#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <random>
#include "Snake.hpp"
#include "Apple.hpp"
#include "Block.hpp"
#include "PowerUp.hpp"
#include "Button.hpp"

class Game {
public:
    Game(int width, int height, const std::string &title);
    void run();

private:
    // ==== CONFIGURACIÓN ====
    const int cellSize  = 24;
    const int maxBlocks = 25;

    // ==== CORE ====
    sf::RenderWindow window;
    int winW, winH;

    enum class State { Menu, Playing, Paused, GameOver, Rules };
    State state;

    // ==== GAME OBJECTS ====
    Snake snake;
    std::vector<Apple> apples;
    std::vector<Block> blocks;
    std::vector<PowerUp> powerups;

    // ==== UI / HUD ====
    sf::Font font;
    sf::Texture menuBgTex;
    sf::Sprite menuBg;
    sf::Music bgMusic;

    std::vector<Button> menuButtons;

    sf::Text applesText;
    sf::Text timeText;
    sf::Text powerupTimersText;

    // ==== GAME DATA ====
    int applesEaten = 0;
    sf::Clock gameClock;
    sf::Time elapsedWhenPaused;

    std::mt19937 rng;

private:
    // ==== METHODS ====
    void processEvents();
    void update(sf::Time dt);
    void render();

    void initMenu();
    void startGame();
    void resetGame();

    void spawnApple();
    void spawnPowerUp();
    void spawnBlocks();
};
