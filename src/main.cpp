#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

const int GRID_SIZE = 20;
const int BASE_WINDOW_WIDTH = 800;
const int BASE_WINDOW_HEIGHT = 600;
const int PANEL_WIDTH = 280;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;
float SCALE_X = 1.0f;
float SCALE_Y = 1.0f;

sf::Texture tex;

enum GameState_Type {
    MENU,
    PLAYING,
    RULES,
    GAME_OVER
};

struct SnakeSegment {
    int x, y;
    
    SnakeSegment(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const SnakeSegment& other) const {
        return x == other.x && y == other.y;
    }
};

struct Block {
    int x, y;
    int width, height;
    
    Block(int x, int y, int width = GRID_SIZE, int height = GRID_SIZE) 
        : x(x), y(y), width(width), height(height) {}
};

enum PowerUpType {
    WALL_PASS,    // Atravesar paredes
    DOUBLE_SCORE, // Puntuación doble
    MAGNET        // Imán que atrae manzanas
};

struct PowerUp {
    int x, y;
    PowerUpType type;
    int width, height;
    
    PowerUp(int x, int y, PowerUpType type) 
        : x(x), y(y), type(type), width(GRID_SIZE), height(GRID_SIZE) {}
};

struct Obstacle {
    int x, y;
    int width, height;
    
    Obstacle(int x, int y) 
        : x(x), y(y), width(GRID_SIZE), height(GRID_SIZE) {}
};

void calculateScaling() {
    SCREEN_WIDTH = WINDOW_WIDTH + PANEL_WIDTH;
    SCREEN_HEIGHT = WINDOW_HEIGHT;
    SCALE_X = (float)WINDOW_WIDTH / BASE_WINDOW_WIDTH;
    SCALE_Y = (float)WINDOW_HEIGHT / BASE_WINDOW_HEIGHT;
}

class Menu {
public:
    int selectedOption = 0;
    
    void handleInput(sf::Keyboard::Scancode key) {
        if (key == sf::Keyboard::Scan::Up && selectedOption > 0) {
            selectedOption--;
        } else if (key == sf::Keyboard::Scan::Down && selectedOption < 2) {
            selectedOption++;
        }
    }
    
    void draw(sf::RenderWindow& window) {
        window.clear(sf::Color::Black);
        
        // Título
        sf::RectangleShape titleBg(sf::Vector2f(SCREEN_WIDTH, 100));
        titleBg.setPosition(0, 50);
        titleBg.setFillColor(sf::Color(50, 50, 50));
        window.draw(titleBg);
        
        // Opciones del menú
        std::vector<std::string> options = {"INICIAR JUEGO", "REGLAS", "SALIR"};
        std::vector<sf::Color> colors = {sf::Color::Red, sf::Color::Yellow, sf::Color::Red};
        
        sf::RectangleShape optionBg3(sf::Vector2f(1080, 600));
        optionBg3.setPosition(sf::Vector2f(0, 0));
        optionBg3.setTexture(&tex);
        window.draw(optionBg3);
        for (int i = 0; i < options.size(); i++) {
            sf::RectangleShape optionBg(sf::Vector2f(300, 60));
            optionBg.setPosition(SCREEN_WIDTH / 2 - 150, 200 + i * 100);
            
            if (i == selectedOption) {
                optionBg.setFillColor(colors[i]);
                optionBg.setOutlineColor(sf::Color::White);
                optionBg.setOutlineThickness(3);
            } else {
                optionBg.setFillColor(sf::Color(50, 50, 50));
                optionBg.setOutlineColor(colors[i]);
                optionBg.setOutlineThickness(2);
            }
            
            window.draw(optionBg);
        }
    }
    
    int getSelectedOption() {
        return selectedOption;
    }
};

class Rules {
public:
    void draw(sf::RenderWindow& window) {
        window.clear(sf::Color::Black);
        
        // Título
        sf::RectangleShape titleBg(sf::Vector2f(SCREEN_WIDTH, 80));
        titleBg.setPosition(0, 20);
        titleBg.setFillColor(sf::Color(50, 50, 50));
        window.draw(titleBg);
        
        // Caja de reglas
        sf::RectangleShape rulesBg(sf::Vector2f(SCREEN_WIDTH - 40, SCREEN_HEIGHT - 120));
        rulesBg.setPosition(20, 120);
        rulesBg.setFillColor(sf::Color(30, 30, 30));
        rulesBg.setOutlineColor(sf::Color::White);
        rulesBg.setOutlineThickness(2);
        window.draw(rulesBg);
        
        // Instrucciones de salida
        sf::RectangleShape exitBg(sf::Vector2f(SCREEN_WIDTH - 40, 50));
        exitBg.setPosition(20, SCREEN_HEIGHT - 60);
        exitBg.setFillColor(sf::Color(50, 50, 0));
        window.draw(exitBg);
    }
};

class GameState {
public:
    std::vector<SnakeSegment> snake;
    std::vector<Block> blocks;
    std::vector<PowerUp> powerUps;
    std::vector<Obstacle> obstacles;
    int score = 0;
    int applesEaten = 0;
    bool gameOver = false;
    int direction = 1;
    int nextDirection = 1;
    float blockSpawnTimer = 0;
    float blockSpawnDelay = 1.0f;
    float powerUpSpawnTimer = 0;
    float powerUpSpawnDelay = 15.0f;
    float obstacleSpawnTimer = 0;
    float obstacleSpawnDelay = 8.0f;
    float gameTimer = 0;
    int speedLevel = 1;
    float moveCounter = 0;
    int moveDelay = 10;
    bool wallPassActive = false;
    float wallPassTimer = 0;
    bool doubleScoreActive = false;
    float doubleScoreTimer = 0;
    bool magnetActive = false;
    float magnetTimer = 0;
    
    GameState() {
        snake.push_back(SnakeSegment(WINDOW_WIDTH / (2 * GRID_SIZE), WINDOW_HEIGHT / (2 * GRID_SIZE)));
    }
    
    void handleInput(sf::Keyboard::Scancode key) {
        if (key == sf::Keyboard::Scan::Up && direction != 2) nextDirection = 0;
        else if (key == sf::Keyboard::Scan::Right && direction != 3) nextDirection = 1;
        else if (key == sf::Keyboard::Scan::Down && direction != 0) nextDirection = 2;
        else if (key == sf::Keyboard::Scan::Left && direction != 1) nextDirection = 3;
    }
    
    void update(float deltaTime) {
        if (gameOver) return;
        
        gameTimer += deltaTime;
        
        speedLevel = 1 + (applesEaten / 10);
        moveDelay = 10 - (speedLevel - 1) * 0.5f;
        if (moveDelay < 2) moveDelay = 2;
        
        if (wallPassActive) {
            wallPassTimer += deltaTime;
            if (wallPassTimer >= 10.0f) {
                wallPassActive = false;
                wallPassTimer = 0;
            }
        }
        
        if (doubleScoreActive) {
            doubleScoreTimer += deltaTime;
            if (doubleScoreTimer >= 10.0f) {
                doubleScoreActive = false;
                doubleScoreTimer = 0;
            }
        }
        
        if (magnetActive) {
            magnetTimer += deltaTime;
            if (magnetTimer >= 10.0f) {
                magnetActive = false;
                magnetTimer = 0;
            }
        }
        
        direction = nextDirection;
        
        if (magnetActive && !snake.empty()) {
            SnakeSegment head = snake[0];
            std::vector<int> blocksToRemove;
            for (int i = 0; i < blocks.size(); i++) {
                auto& block = blocks[i];
                int blockGridX = block.x / GRID_SIZE;
                int blockGridY = block.y / GRID_SIZE;

                // Atraer hacia la cabeza
                if (blockGridX < head.x) blockGridX++;
                else if (blockGridX > head.x) blockGridX--;

                if (blockGridY < head.y) blockGridY++;
                else if (blockGridY > head.y) blockGridY--;

                block.x = blockGridX * GRID_SIZE;
                block.y = blockGridY * GRID_SIZE;

                // Comer automáticamente si llega a la cabeza
                if (block.x == head.x * GRID_SIZE && block.y == head.y * GRID_SIZE) {
                    int points = doubleScoreActive ? 20 : 10;
                    score += points;
                    applesEaten++;
                    blocksToRemove.push_back(i);
                }
            }
            for (int i = blocksToRemove.size() - 1; i >= 0; i--) {
                blocks.erase(blocks.begin() + blocksToRemove[i]);
            }
        }
        
        moveCounter += speedLevel;
        if (moveCounter < moveDelay) {
            powerUpSpawnTimer += 0.016f;
            if (powerUpSpawnTimer >= powerUpSpawnDelay) {
                int randomX = rand() % (WINDOW_WIDTH / GRID_SIZE);
                int randomY = rand() % (WINDOW_HEIGHT / GRID_SIZE);
                bool validPos = true;
                
                for (const auto& segment : snake) {
                    if (segment.x == randomX && segment.y == randomY) {
                        validPos = false;
                        break;
                    }
                }
                
                if (validPos) {
                    PowerUpType type = (rand() % 3 == 0) ? WALL_PASS : (rand() % 2 == 0) ? DOUBLE_SCORE : MAGNET;
                    powerUps.push_back(PowerUp(randomX * GRID_SIZE, randomY * GRID_SIZE, type));
                }
                powerUpSpawnTimer = 0;
            }
            
            obstacleSpawnTimer += 0.016f;
            if (obstacleSpawnTimer >= obstacleSpawnDelay) {
                int randomX = rand() % (WINDOW_WIDTH / GRID_SIZE);
                int randomY = rand() % (WINDOW_HEIGHT / GRID_SIZE);
                bool validPos = true;
                
                for (const auto& segment : snake) {
                    if (segment.x == randomX && segment.y == randomY) {
                        validPos = false;
                        break;
                    }
                }
                
                if (validPos && obstacles.size() < 20) {
                    obstacles.push_back(Obstacle(randomX * GRID_SIZE, randomY * GRID_SIZE));
                }
                obstacleSpawnTimer = 0;
            }
            return;
        }
        moveCounter = 0;
        
        SnakeSegment head = snake[0];
        if (direction == 0) head.y--;
        else if (direction == 1) head.x++;
        else if (direction == 2) head.y++;
        else if (direction == 3) head.x--;
        
        if (!wallPassActive) {
            if (head.x < 0 || head.x >= WINDOW_WIDTH / GRID_SIZE ||
                head.y < 0 || head.y >= WINDOW_HEIGHT / GRID_SIZE) {
                gameOver = true;
                return;
            }
        } else {
            if (head.x < 0) head.x = WINDOW_WIDTH / GRID_SIZE - 1;
            else if (head.x >= WINDOW_WIDTH / GRID_SIZE) head.x = 0;
            if (head.y < 0) head.y = WINDOW_HEIGHT / GRID_SIZE - 1;
            else if (head.y >= WINDOW_HEIGHT / GRID_SIZE) head.y = 0;
        }
        
        for (const auto& segment : snake) {
            if (head == segment) {
                gameOver = true;
                return;
            }
        }
        
        for (const auto& obstacle : obstacles) {
            if (head.x * GRID_SIZE == obstacle.x && head.y * GRID_SIZE == obstacle.y) {
                gameOver = true;
                return;
            }
        }
        
        snake.insert(snake.begin(), head);
        
        bool ateBlock = false;
        for (auto it = blocks.begin(); it != blocks.end(); ++it) {
            if (head.x * GRID_SIZE == it->x && head.y * GRID_SIZE == it->y) {
                int points = doubleScoreActive ? 20 : 10;
                score += points;
                applesEaten++;
                blocks.erase(it);
                ateBlock = true;
                break;
            }
        }
        
        for (auto it = powerUps.begin(); it != powerUps.end(); ++it) {
            if (head.x * GRID_SIZE == it->x && head.y * GRID_SIZE == it->y) {
                if (it->type == WALL_PASS) {
                    wallPassActive = true;
                    wallPassTimer = 0;
                } else if (it->type == DOUBLE_SCORE) {
                    doubleScoreActive = true;
                    doubleScoreTimer = 0;
                } else if (it->type == MAGNET) {
                    magnetActive = true;
                    magnetTimer = 0;
                }
                powerUps.erase(it);
                break;
            }
        }
        
        if (!ateBlock && snake.size() > 1) {
            snake.pop_back();
        }
        
        blockSpawnTimer += 0.016f;
        if (blockSpawnTimer >= blockSpawnDelay) {
            int randomX = rand() % (WINDOW_WIDTH / GRID_SIZE);
            int randomY = rand() % (WINDOW_HEIGHT / GRID_SIZE);
            
            bool onSnake = false;
            for (const auto& segment : snake) {
                if (segment.x == randomX && segment.y == randomY) {
                    onSnake = true;
                    break;
                }
            }
            
            if (!onSnake) {
                blocks.push_back(Block(randomX * GRID_SIZE, randomY * GRID_SIZE));
            }
            
            blockSpawnTimer = 0;
        }
    }
    
    void draw(sf::RenderWindow& window) {
        for (const auto& segment : snake) {
            sf::RectangleShape rect(sf::Vector2f((GRID_SIZE - 2) * SCALE_X, (GRID_SIZE - 2) * SCALE_Y));
            rect.setPosition(segment.x * GRID_SIZE * SCALE_X + SCALE_X, segment.y * GRID_SIZE * SCALE_Y + SCALE_Y);
            rect.setFillColor(sf::Color::Green);
            window.draw(rect);
        }
        
        for (const auto& block : blocks) {
            sf::RectangleShape rect(sf::Vector2f((block.width - 2) * SCALE_X, (block.height - 2) * SCALE_Y));
            rect.setPosition(block.x * SCALE_X + SCALE_X, block.y * SCALE_Y + SCALE_Y);
            rect.setFillColor(sf::Color::Red);
            window.draw(rect);
        }
        
        for (const auto& powerUp : powerUps) {
            sf::RectangleShape rect(sf::Vector2f((powerUp.width - 2) * SCALE_X, (powerUp.height - 2) * SCALE_Y));
            rect.setPosition(powerUp.x * SCALE_X + SCALE_X, powerUp.y * SCALE_Y + SCALE_Y);
            if (powerUp.type == WALL_PASS) {
                rect.setFillColor(sf::Color::Yellow);
            } else if (powerUp.type == DOUBLE_SCORE) {
                rect.setFillColor(sf::Color::Magenta);
            } else if (powerUp.type == MAGNET) {
                rect.setFillColor(sf::Color(255, 165, 0));
            }
            window.draw(rect);
        }
        
        for (const auto& obstacle : obstacles) {
            sf::RectangleShape rect(sf::Vector2f((obstacle.width - 2) * SCALE_X, (obstacle.height - 2) * SCALE_Y));
            rect.setPosition(obstacle.x * SCALE_X + SCALE_X, obstacle.y * SCALE_Y + SCALE_Y);
            rect.setFillColor(sf::Color::Cyan);
            window.draw(rect);
        }
        
        sf::RectangleShape dividerLine(sf::Vector2f(2, WINDOW_HEIGHT * SCALE_Y));
        dividerLine.setPosition(WINDOW_WIDTH * SCALE_X, 0);
        dividerLine.setFillColor(sf::Color::White);
        window.draw(dividerLine);
    }
    
    void drawUI(sf::RenderWindow& window) {
        int panelStartX = WINDOW_WIDTH * SCALE_X;
        
        sf::RectangleShape infoBg(sf::Vector2f(PANEL_WIDTH - 10, WINDOW_HEIGHT * SCALE_Y));
        infoBg.setPosition(panelStartX + 5, 0);
        infoBg.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(infoBg);
        
        int panelX = panelStartX + 15;
        int yPos = 10;
        
        sf::RectangleShape separator(sf::Vector2f(PANEL_WIDTH - 20, 1));
        separator.setPosition(panelX, yPos + 25);
        separator.setFillColor(sf::Color::White);
        window.draw(separator);
        
        sf::RectangleShape scoreBg(sf::Vector2f(PANEL_WIDTH - 20, 25));
        scoreBg.setPosition(panelX, yPos);
        scoreBg.setFillColor(sf::Color(50, 50, 50));
        window.draw(scoreBg);
        
        int scoreBarWidth = (score / 10) % (PANEL_WIDTH - 20);
        sf::RectangleShape scoreBar(sf::Vector2f(scoreBarWidth, 3));
        scoreBar.setPosition(panelX, yPos + 22);
        scoreBar.setFillColor(sf::Color::Green);
        window.draw(scoreBar);
        
        yPos += 35;
        
        sf::RectangleShape applesBox(sf::Vector2f(50, 18));
        applesBox.setPosition(panelX, yPos);
        applesBox.setFillColor(sf::Color(100, 0, 0));
        applesBox.setOutlineColor(sf::Color::Red);
        applesBox.setOutlineThickness(2);
        window.draw(applesBox);
        
        sf::RectangleShape appleIndicator(sf::Vector2f(8, 8));
        appleIndicator.setPosition(panelX + 5, yPos + 5);
        appleIndicator.setFillColor(sf::Color::Red);
        window.draw(appleIndicator);
        
        int cubesPerRow = 10;
        for (int i = 0; i < applesEaten && i < 50; i++) {
            int xPos = panelX + 5 + (i % cubesPerRow) * 12;
            int yPos_cube = yPos + 4 + (i / cubesPerRow) * 12;
            sf::RectangleShape cube(sf::Vector2f(8, 8));
            cube.setPosition(xPos, yPos_cube);
            cube.setFillColor(sf::Color::Red);
            cube.setOutlineColor(sf::Color::White);
            cube.setOutlineThickness(1);
            window.draw(cube);
        }
        
        // Mostrar puntos por manzana
        int pointsPerApple = doubleScoreActive ? 20 : 10;

        sf::RectangleShape pointsBox(sf::Vector2f(PANEL_WIDTH - 20, 22));
        pointsBox.setPosition(panelX, yPos);
        pointsBox.setFillColor(sf::Color(0, 120, 0));
        pointsBox.setOutlineColor(sf::Color::Green);
        pointsBox.setOutlineThickness(1);
        window.draw(pointsBox);

        // Dibuja barras pequeñas para representar el valor
        for (int i = 0; i < pointsPerApple / 10; i++) {
            sf::RectangleShape pointBar(sf::Vector2f(4, 15));
            pointBar.setPosition(panelX + 5 + i * 6, yPos + 3);
            pointBar.setFillColor(sf::Color::Green);
            window.draw(pointBar);
        }

        yPos += 28;
        
        sf::RectangleShape speedLabel(sf::Vector2f(PANEL_WIDTH - 20, 3));
        speedLabel.setPosition(panelX, yPos);
        speedLabel.setFillColor(sf::Color::Yellow);
        window.draw(speedLabel);
        
        for (int i = 0; i < speedLevel && i < 8; i++) {
            sf::RectangleShape speedBar(sf::Vector2f(8, 12));
            speedBar.setPosition(panelX + i * 10, yPos + 8);
            speedBar.setFillColor(sf::Color::Yellow);
            window.draw(speedBar);
        }
        
        yPos += 30;
        
        if (wallPassActive) {
            sf::RectangleShape wallPassBg(sf::Vector2f(PANEL_WIDTH - 20, 35));
            wallPassBg.setPosition(panelX, yPos);
            wallPassBg.setFillColor(sf::Color(100, 100, 0));
            window.draw(wallPassBg);
            
            float wallPassProgress = wallPassTimer / 10.0f;
            sf::RectangleShape wallPassBar(sf::Vector2f((PANEL_WIDTH - 20) * (1.0f - wallPassProgress), 5));
            wallPassBar.setPosition(panelX, yPos + 28);
            wallPassBar.setFillColor(sf::Color::Yellow);
            window.draw(wallPassBar);
            
            sf::RectangleShape wallPassBorder(sf::Vector2f(PANEL_WIDTH - 20, 35));
            wallPassBorder.setPosition(panelX, yPos);
            wallPassBorder.setFillColor(sf::Color::Transparent);
            wallPassBorder.setOutlineColor(sf::Color::Yellow);
            wallPassBorder.setOutlineThickness(2);
            window.draw(wallPassBorder);
            
            yPos += 40;
        }
        
        if (doubleScoreActive) {
            sf::RectangleShape doubleScoreBg(sf::Vector2f(PANEL_WIDTH - 20, 35));
            doubleScoreBg.setPosition(panelX, yPos);
            doubleScoreBg.setFillColor(sf::Color(100, 0, 100));
            window.draw(doubleScoreBg);
            
            float doubleScoreProgress = doubleScoreTimer / 10.0f;
            sf::RectangleShape doubleScoreBar(sf::Vector2f((PANEL_WIDTH - 20) * (1.0f - doubleScoreProgress), 5));
            doubleScoreBar.setPosition(panelX, yPos + 28);
            doubleScoreBar.setFillColor(sf::Color::Magenta);
            window.draw(doubleScoreBar);
            
            sf::RectangleShape doubleScoreBorder(sf::Vector2f(PANEL_WIDTH - 20, 35));
            doubleScoreBorder.setPosition(panelX, yPos);
            doubleScoreBorder.setFillColor(sf::Color::Transparent);
            doubleScoreBorder.setOutlineColor(sf::Color::Magenta);
            doubleScoreBorder.setOutlineThickness(2);
            window.draw(doubleScoreBorder);
            
            yPos += 40;
        }
        
        if (magnetActive) {
            sf::RectangleShape magnetBg(sf::Vector2f(PANEL_WIDTH - 20, 35));
            magnetBg.setPosition(panelX, yPos);
            magnetBg.setFillColor(sf::Color(165, 100, 0));
            window.draw(magnetBg);
            
            float magnetProgress = magnetTimer / 10.0f;
            sf::RectangleShape magnetBar(sf::Vector2f((PANEL_WIDTH - 20) * (1.0f - magnetProgress), 5));
            magnetBar.setPosition(panelX, yPos + 28);
            magnetBar.setFillColor(sf::Color(255, 165, 0));
            window.draw(magnetBar);
            
            sf::RectangleShape magnetBorder(sf::Vector2f(PANEL_WIDTH - 20, 35));
            magnetBorder.setPosition(panelX, yPos);
            magnetBorder.setFillColor(sf::Color::Transparent);
            magnetBorder.setOutlineColor(sf::Color(255, 165, 0));
            magnetBorder.setOutlineThickness(2);
            window.draw(magnetBorder);
        }
    }
};

class GameOverMenu {
public:
    bool isVisible = false;
    int finalScore = 0;
    int finalApplesEaten = 0;
    int selectedOption = 0;  // 0 = Reiniciar, 1 = Salir
    
    void show(int score, int applesEaten) {
        isVisible = true;
        finalScore = score;
        finalApplesEaten = applesEaten;
        selectedOption = 0;
    }
    
    void handleInput(sf::Keyboard::Scancode key) {
        if (key == sf::Keyboard::Scan::Left || key == sf::Keyboard::Scan::Up) {
            selectedOption = 0;
        } else if (key == sf::Keyboard::Scan::Right || key == sf::Keyboard::Scan::Down) {
            selectedOption = 1;
        }
    }
    
    void draw(sf::RenderWindow& window) {
        if (!isVisible) return;
        
        // Fondo oscuro semitransparente
        sf::RectangleShape background(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
        background.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(background);
        
        // Cuadro principal
        int menuWidth = 500;
        int menuHeight = 400;
        int menuX = (SCREEN_WIDTH - menuWidth) / 2;
        int menuY = (SCREEN_HEIGHT - menuHeight) / 2;
        
        sf::RectangleShape menuBox(sf::Vector2f(menuWidth, menuHeight));
        menuBox.setPosition(menuX, menuY);
        menuBox.setFillColor(sf::Color(30, 30, 30));
        menuBox.setOutlineColor(sf::Color::Red);
        menuBox.setOutlineThickness(3);
        window.draw(menuBox);
        
        // Título "GAME OVER"
        sf::RectangleShape titleBg(sf::Vector2f(menuWidth, 60));
        titleBg.setPosition(menuX, menuY);
        titleBg.setFillColor(sf::Color::Red);
        window.draw(titleBg);
        
        // Puntaje
        int yOffset = menuY + 80;
        
        sf::RectangleShape scoreLabel(sf::Vector2f(150, 20));
        scoreLabel.setPosition(menuX + 20, yOffset);
        scoreLabel.setFillColor(sf::Color::Green);
        window.draw(scoreLabel);
        
        std::ostringstream scoreStr;
        scoreStr << "SCORE: " << finalScore;
        
        // Mostrar score (usando líneas para simular números)
        yOffset += 30;
        sf::RectangleShape scoreLine(sf::Vector2f(300, 2));
        scoreLine.setPosition(menuX + 100, yOffset);
        scoreLine.setFillColor(sf::Color::Green);
        window.draw(scoreLine);
        
        // Manzanas comidas
        yOffset += 40;
        sf::RectangleShape applesLabel(sf::Vector2f(200, 20));
        applesLabel.setPosition(menuX + 20, yOffset);
        applesLabel.setFillColor(sf::Color::Yellow);
        window.draw(applesLabel);
        
        std::ostringstream applesStr;
        applesStr << "MANZANAS: " << finalApplesEaten;
        
        yOffset += 30;
        sf::RectangleShape applesLine(sf::Vector2f(300, 2));
        applesLine.setPosition(menuX + 100, yOffset);
        applesLine.setFillColor(sf::Color::Yellow);
        window.draw(applesLine);
        
        // Botones: Reiniciar y Salir
        yOffset += 50;
        
        // Botón Reiniciar
        sf::RectangleShape restartBtn(sf::Vector2f(150, 50));
        restartBtn.setPosition(menuX + 50, yOffset);
        restartBtn.setFillColor(selectedOption == 0 ? sf::Color::Green : sf::Color(0, 100, 0));
        restartBtn.setOutlineColor(selectedOption == 0 ? sf::Color::White : sf::Color::Green);
        restartBtn.setOutlineThickness(2);
        window.draw(restartBtn);
        
        // Botón Salir
        sf::RectangleShape exitBtn(sf::Vector2f(150, 50));
        exitBtn.setPosition(menuX + 300, yOffset);
        exitBtn.setFillColor(selectedOption == 1 ? sf::Color::Red : sf::Color(100, 0, 0));
        exitBtn.setOutlineColor(selectedOption == 1 ? sf::Color::Black : sf::Color::Black);
        exitBtn.setOutlineThickness(2);
        window.draw(exitBtn);
        
        // Labels de botones (usando líneas)
        yOffset += 15;
        
        // "REINICIAR" en botón izquierdo
        for (int i = 0; i < 4; i++) {
            sf::RectangleShape line(sf::Vector2f(30, 2));
            line.setPosition(menuX + 60 + i * 25, yOffset);
            line.setFillColor(sf::Color::White);
            window.draw(line);
        }
        
        // "SALIR" en botón derecho
        for (int i = 0; i < 3; i++) {
            sf::RectangleShape line(sf::Vector2f(30, 2));
            line.setPosition(menuX + 310 + i * 25, yOffset);
            line.setFillColor(sf::Color::White);
            window.draw(line);
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));
    
    calculateScaling();
    
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Snake vs Blocks");
    window.setFramerateLimit(60);
    
    tex.loadFromFile("C:/Users/Bienvenido/Desktop/SNAKEvsBLOCE/assets/images/Menu.Fondo.png.png");
    GameState_Type gameState = MENU;
    Menu menu;
    Rules rules;
    GameState game;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.scancode == sf::Keyboard::Scan::Escape) {
                    if (gameState == PLAYING || gameState == GAME_OVER) {
                        gameState = MENU;
                        game = GameState();
                    } else if (gameState == RULES) {
                        gameState = MENU;
                    } else {
                        window.close();
                    }
                }
                
                if (gameState == MENU) {
                    menu.handleInput(event.key.scancode);
                    
                    if (event.key.scancode == sf::Keyboard::Scan::Enter) {
                        int option = menu.getSelectedOption();
                        if (option == 0) {
                            gameState = PLAYING;
                            game = GameState();
                        } else if (option == 1) {
                            gameState = RULES;
                        } else if (option == 2) {
                            window.close();
                        }
                    }
                } else if (gameState == RULES) {
                    if (event.key.scancode == sf::Keyboard::Scan::Enter) {
                        gameState = MENU;
                        menu.selectedOption = 0;
                    }
                } else if (gameState == PLAYING) {
                    if (event.key.scancode == sf::Keyboard::Scan::R && game.gameOver) {
                        game = GameState();
                    }
                    game.handleInput(event.key.scancode);
                }
            }
        }
        
        if (gameState == MENU) {
            menu.draw(window);
        } else if (gameState == RULES) {
            rules.draw(window);
        } else if (gameState == PLAYING) {
            game.update(0.016f);
            
            window.clear(sf::Color::Black);
            game.draw(window);
            game.drawUI(window);
        }
        
        window.display();
    }
    
    return 0;
}