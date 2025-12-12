#include "Game.hpp"
#include <sstream>
#include <iostream>

Game::Game(int width, int height, const std::string &title)
: window(sf::VideoMode(width, height), title), winW(width), winH(height), state(State::Menu), snake(cellSize)
{
    std::random_device rd; 
    rng = std::mt19937(rd());

    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "ERROR: Missing font assets/arial.ttf\n";
    }

    if (menuBgTex.loadFromFile("assets/menu_bg.png")) {
        menuBg.setTexture(menuBgTex);
        menuBg.setScale(
            float(winW) / menuBgTex.getSize().x,
            float(winH) / menuBgTex.getSize().y
        );
    }

    if (bgMusic.openFromFile("assets/music.ogg")) {
        bgMusic.setLoop(true);
        bgMusic.play();
    }

    applesText.setFont(font);
    applesText.setCharacterSize(20);
    applesText.setPosition(winW - 220.f, 8.f);

    timeText.setFont(font);
    timeText.setCharacterSize(20);
    timeText.setPosition(winW - 420.f, 8.f);

    powerupTimersText.setFont(font);
    powerupTimersText.setCharacterSize(18);
    powerupTimersText.setPosition(winW - 420.f, 36.f);

    initMenu();
    resetGame();
}

void Game::initMenu()
{
    float bw = 220.f, bh = 60.f;

    menuButtons.clear();
    menuButtons.emplace_back(sf::Vector2f(winW/2.f - bw/2.f, winH/2.f - 80.f), bw, bh, "Iniciar", font);
    menuButtons.emplace_back(sf::Vector2f(winW/2.f - bw/2.f, winH/2.f),      bw, bh, "Reglas", font);
    menuButtons.emplace_back(sf::Vector2f(winW/2.f - bw/2.f, winH/2.f + 80.f), bw, bh, "Salir", font);
}

void Game::resetGame()
{
    snake.reset(sf::Vector2i(10,10));

    apples.clear();
    blocks.clear();
    powerups.clear();

    applesEaten = 0;
    gameClock.restart();
    elapsedWhenPaused = sf::Time::Zero;

    for (int i=0;i<6;i++) spawnApple();
    spawnBlocks();
}

void Game::startGame()
{
    resetGame();
    state = State::Playing;
}

void Game::run()
{
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time dt = clock.restart();

        processEvents();

        if (state == State::Playing)
            update(dt);

        render();
    }
}

void Game::processEvents()
{
    sf::Event e;

    while (window.pollEvent(e)) {

        if (e.type == sf::Event::Closed)
            window.close();

        if (state == State::Menu) 
        {
            if (e.type == sf::Event::MouseButtonPressed) 
            {
                sf::Vector2f mouse(sf::Mouse::getPosition(window));

                if (menuButtons[0].contains(mouse)) startGame();
                if (menuButtons[1].contains(mouse)) state = State::Rules;
                if (menuButtons[2].contains(mouse)) window.close();
            }
        }

        else if (state == State::Playing) 
        {
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::P)
                state = State::Paused;

            snake.handleEvent(e);
        }

        else if (state == State::Paused) 
        {
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::P) state = State::Playing;
                if (e.key.code == sf::Keyboard::R) startGame();
                if (e.key.code == sf::Keyboard::Escape) state = State::Menu;
            }
        }

        else if (state == State::Rules) 
        {
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
                state = State::Menu;
        }

        else if (state == State::GameOver) 
        {
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::R) startGame();
                if (e.key.code == sf::Keyboard::Escape) state = State::Menu;
            }
        }
    }
}

void Game::update(sf::Time dt)
{
    snake.update(dt);

    for (int i=int(apples.size())-1;i>=0;i--) 
    {
        if (snake.checkAppleCollision(apples[i].pos)) 
        {
            int add = (snake.activeDouble ? 2 : 1);
            snake.grow(add);
            applesEaten += add;

            apples.erase(apples.begin()+i);
            spawnApple();

            if (applesEaten % 5 == 0)
                snake.increaseSpeed(20.f);
        }
    }

    if (snake.activeMagnet) 
    {
        sf::Vector2i head(
            int(snake.getHeadPositionF().x / cellSize),
            int(snake.getHeadPositionF().y / cellSize)
        );

        for (auto &a : apples)
        {
            int dx = head.x - a.pos.x;
            int dy = head.y - a.pos.y;
            int d2 = dx*dx + dy*dy;

            if (d2 <= 15*15)
            {
                if (dx < 0) a.pos.x++;
                if (dx > 0) a.pos.x--;
                if (dy < 0) a.pos.y++;
                if (dy > 0) a.pos.y--;
            }
        }
    }

    for (int i=int(powerups.size())-1;i>=0;i--)
    {
        if (snake.checkAppleCollision(powerups[i].pos))
        {
            powerups[i].applyTo(snake);
            powerups.erase(powerups.begin()+i);
        }
    }

    if (!snake.activePhase)
    {
        for (auto &b : blocks)
        {
            for (int ox=0; ox<b.size; ox++)
            for (int oy=0; oy<b.size; oy++)
            {
                if (snake.checkCollision(b.pos + sf::Vector2i(ox,oy)))
                {
                    state = State::GameOver;
                    return;
                }
            }
        }
    }

    if (!snake.activePhase)
    {
        if (!snake.withinBounds(winW/cellSize, winH/cellSize))
        {
            state = State::GameOver;
            return;
        }
    }
    else
    {
        snake.wrapPosition(winW/cellSize, winH/cellSize);
    }

    if (snake.checkSelfCollision())
    {
        state = State::GameOver;
        return;
    }

    if ((rng() % 1000) < 3)
        spawnPowerUp();

    std::ostringstream ss;
    if (snake.activeMagnet) ss << "Magnet: " << int(std::ceil(snake.magnetRemaining.asSeconds())) << "s\n";
    if (snake.activeDouble) ss << "x2: " << int(std::ceil(snake.doubleRemaining.asSeconds())) << "s\n";
    if (snake.activePhase)  ss << "Phase: " << int(std::ceil(snake.phaseRemaining.asSeconds()))  << "s\n";

    powerupTimersText.setString(ss.str());

    applesText.setString("Apples: " + std::to_string(applesEaten));

    sf::Time played = gameClock.getElapsedTime() + elapsedWhenPaused;
    timeText.setString("Time: " + std::to_string(int(played.asSeconds())) + "s");
}

void Game::render()
{
    window.clear();

    if (state == State::Menu) 
    {
        if (menuBgTex.getSize().x > 0)
            window.draw(menuBg);

        for (auto &b : menuButtons)
            b.draw(window);
    }

    else if (state == State::Rules) 
    {
        sf::Text t(
            "Reglas:\n"
            "- Come manzanas\n"
            "- Evita bloques y paredes\n"
            "- Powerups: Iman, x2, Phase\n\n"
            "Presiona ESC para volver",
            font, 20
        );
        t.setPosition(40,40);
        window.draw(t);
    }

    else 
    {
        for (auto &a : apples)   a.draw(window, cellSize);
        for (auto &b : blocks)   b.draw(window, cellSize);
        for (auto &p : powerups) p.draw(window, cellSize);

        snake.draw(window, cellSize);

        window.draw(applesText);
        window.draw(timeText);
        window.draw(powerupTimersText);

        if (state == State::Paused) {
            sf::Text t("PAUSA\nP para reanudar\nR reiniciar\nESC menu", font, 28);
            t.setPosition(winW/2.f - 150, winH/2.f - 80);
            window.draw(t);
        }

        if (state == State::GameOver) {
            sf::Text t("GAME OVER\nR reiniciar\nESC menu", font, 30);
            t.setPosition(winW/2.f - 170, winH/2.f - 70);
            window.draw(t);

            sf::Text stats(
                "Tiempo: " + std::to_string(int(gameClock.getElapsedTime().asSeconds())) + "s\n"
                "Manzanas: " + std::to_string(applesEaten),
                font, 22
            );
            stats.setPosition(winW/2.f - 150, winH/2.f + 40);
            window.draw(stats);
        }
    }

    window.display();
}

void Game::spawnApple()
{
    std::uniform_int_distribution<int> dx(1, winW/cellSize - 2);
    std::uniform_int_distribution<int> dy(1, winH/cellSize - 2);

    Apple a;
    a.pos = sf::Vector2i(dx(rng), dy(rng));
    apples.push_back(a);
}

void Game::spawnPowerUp()
{
    std::uniform_int_distribution<int> dx(1, winW/cellSize - 2);
    std::uniform_int_distribution<int> dy(1, winH/cellSize - 2);

    PowerUp p;
    int k = rng() % 3;

    if (k==0) p.type = PowerUp::Type::Magnet;
    if (k==1) p.type = PowerUp::Type::Double;
    if (k==2) p.type = PowerUp::Type::Phase;

    p.pos = sf::Vector2i(dx(rng), dy(rng));
    powerups.push_back(p);
}

void Game::spawnBlocks()
{
    std::uniform_int_distribution<int> dx(1, winW/cellSize - 4);
    std::uniform_int_distribution<int> dy(1, winH/cellSize - 4);
    std::uniform_int_distribution<int> sz(1,3);

    for (int i=0;i<maxBlocks;i++) 
    {
        Block b;
        b.pos = sf::Vector2i(dx(rng), dy(rng));
        b.size = sz(rng);
        blocks.push_back(b);
    }
}

