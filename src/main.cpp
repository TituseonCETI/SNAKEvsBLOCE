#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

const int GRID_SIZE = 20;
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

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
    DOUBLE_SCORE  // Puntuación doble
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

class GameState {
public:
    std::vector<SnakeSegment> snake;
    std::vector<Block> blocks;
    std::vector<PowerUp> powerUps;
    std::vector<Obstacle> obstacles;
    int score = 0;
    int applesEaten = 0;
    bool gameOver = false;
    int direction = 1; // 0=arriba, 1=derecha, 2=abajo, 3=izquierda
    int nextDirection = 1;
    float blockSpawnTimer = 0;
    float blockSpawnDelay = 1.0f;
    float powerUpSpawnTimer = 0;
    float powerUpSpawnDelay = 5.0f;
    float obstacleSpawnTimer = 0;
    float obstacleSpawnDelay = 3.0f;
    float gameTimer = 0;
    int speedLevel = 1;
    float moveCounter = 0;
    int moveDelay = 10;
    int windowExpansionCount = 0;
    bool wallPassActive = false;
    float wallPassTimer = 0;
    bool doubleScoreActive = false;
    float doubleScoreTimer = 0;
    
    GameState() {
        // Inicializar serpiente en el medio
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
        
        // Aumentar velocidad cada 10 segundos
        speedLevel = 1 + (int)(gameTimer / 10.0f);
        moveDelay = 10 - (speedLevel - 1) * 1;
        if (moveDelay < 3) moveDelay = 3;
        
        // Actualizar timers de power-ups
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
        
        direction = nextDirection;
        
        // Contador de movimiento basado en velocidad
        moveCounter += speedLevel;
        if (moveCounter < moveDelay) {
            // Generar power-ups
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
                    PowerUpType type = (rand() % 2 == 0) ? WALL_PASS : DOUBLE_SCORE;
                    powerUps.push_back(PowerUp(randomX * GRID_SIZE, randomY * GRID_SIZE, type));
                }
                powerUpSpawnTimer = 0;
            }
            
            // Generar obstáculos
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
                
                if (validPos && obstacles.size() < 5) {
                    obstacles.push_back(Obstacle(randomX * GRID_SIZE, randomY * GRID_SIZE));
                }
                obstacleSpawnTimer = 0;
            }
            return;
        }
        moveCounter = 0;
        
        // Mover serpiente
        SnakeSegment head = snake[0];
        if (direction == 0) head.y--;
        else if (direction == 1) head.x++;
        else if (direction == 2) head.y++;
        else if (direction == 3) head.x--;
        
        // Colisión con paredes
        if (!wallPassActive) {
            if (head.x < 0 || head.x >= WINDOW_WIDTH / GRID_SIZE ||
                head.y < 0 || head.y >= WINDOW_HEIGHT / GRID_SIZE) {
                gameOver = true;
                return;
            }
        } else {
            // Atravesar paredes
            if (head.x < 0) head.x = WINDOW_WIDTH / GRID_SIZE - 1;
            else if (head.x >= WINDOW_WIDTH / GRID_SIZE) head.x = 0;
            if (head.y < 0) head.y = WINDOW_HEIGHT / GRID_SIZE - 1;
            else if (head.y >= WINDOW_HEIGHT / GRID_SIZE) head.y = 0;
        }
        
        // Colisión consigo misma
        for (const auto& segment : snake) {
            if (head == segment) {
                gameOver = true;
                return;
            }
        }
        
        // Colisión con obstáculos
        for (const auto& obstacle : obstacles) {
            if (head.x * GRID_SIZE == obstacle.x && head.y * GRID_SIZE == obstacle.y) {
                gameOver = true;
                return;
            }
        }
        
        snake.insert(snake.begin(), head);
        
        // Comprobar colisión con bloques (manzanas)
        bool ateBlock = false;
        for (auto it = blocks.begin(); it != blocks.end(); ++it) {
            if (head.x * GRID_SIZE == it->x && head.y * GRID_SIZE == it->y) {
                int points = doubleScoreActive ? 20 : 10;
                score += points;
                applesEaten++;
                blocks.erase(it);
                ateBlock = true;
                
                // Expandir pantalla cada 5 manzanas
                if (applesEaten % 5 == 0) {
                    WINDOW_WIDTH += 100;
                    WINDOW_HEIGHT += 100;
                    windowExpansionCount++;
                }
                break;
            }
        }
        
        // Comprobar colisión con power-ups
        for (auto it = powerUps.begin(); it != powerUps.end(); ++it) {
            if (head.x * GRID_SIZE == it->x && head.y * GRID_SIZE == it->y) {
                if (it->type == WALL_PASS) {
                    wallPassActive = true;
                    wallPassTimer = 0;
                } else if (it->type == DOUBLE_SCORE) {
                    doubleScoreActive = true;
                    doubleScoreTimer = 0;
                }
                powerUps.erase(it);
                break;
            }
        }
        
        // La serpiente siempre crece (no elimina cola si comió)
        // Si no comió, elimina la cola para mantener tamaño
        if (!ateBlock && snake.size() > 1) {
            snake.pop_back();
        }
        
        // Generar bloques aleatoriamente
        blockSpawnTimer += 0.016f;
        if (blockSpawnTimer >= blockSpawnDelay) {
            int randomX = rand() % (WINDOW_WIDTH / GRID_SIZE);
            int randomY = rand() % (WINDOW_HEIGHT / GRID_SIZE);
            
            // Evitar que aparezcan en la serpiente
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
        // Dibujar serpiente
        for (const auto& segment : snake) {
            sf::RectangleShape rect(sf::Vector2f(GRID_SIZE - 2, GRID_SIZE - 2));
            rect.setPosition(segment.x * GRID_SIZE + 1, segment.y * GRID_SIZE + 1);
            rect.setFillColor(sf::Color::Green);
            window.draw(rect);
        }
        
        // Dibujar bloques (manzanas)
        for (const auto& block : blocks) {
            sf::RectangleShape rect(sf::Vector2f(block.width - 2, block.height - 2));
            rect.setPosition(block.x + 1, block.y + 1);
            rect.setFillColor(sf::Color::Red);
            window.draw(rect);
        }
        
        // Dibujar power-ups
        for (const auto& powerUp : powerUps) {
            sf::RectangleShape rect(sf::Vector2f(powerUp.width - 2, powerUp.height - 2));
            rect.setPosition(powerUp.x + 1, powerUp.y + 1);
            if (powerUp.type == WALL_PASS) {
                rect.setFillColor(sf::Color::Yellow);
            } else {
                rect.setFillColor(sf::Color::Magenta);
            }
            window.draw(rect);
        }
        
        // Dibujar obstáculos
        for (const auto& obstacle : obstacles) {
            sf::RectangleShape rect(sf::Vector2f(obstacle.width - 2, obstacle.height - 2));
            rect.setPosition(obstacle.x + 1, obstacle.y + 1);
            rect.setFillColor(sf::Color::Cyan);
            window.draw(rect);
        }
    }
    
    void drawUI(sf::RenderWindow& window) {
        // Fondo para información en la esquina superior izquierda
        sf::RectangleShape infoBg(sf::Vector2f(250, 150));
        infoBg.setPosition(5, 5);
        infoBg.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(infoBg);
        
        // Indicadores de estado
        // Puntuación
        sf::RectangleShape scoreLabel(sf::Vector2f(50, 10));
        scoreLabel.setPosition(10, 10);
        scoreLabel.setFillColor(sf::Color::White);
        window.draw(scoreLabel);
        
        // Manzanas comidas
        sf::RectangleShape applesLabel(sf::Vector2f(10, 10));
        applesLabel.setPosition(10, 30);
        applesLabel.setFillColor(sf::Color::Red);
        window.draw(applesLabel);
        
        // Speed level
        sf::RectangleShape speedLabel(sf::Vector2f(30, 5));
        speedLabel.setPosition(10, 50);
        speedLabel.setFillColor(sf::Color::White);
        window.draw(speedLabel);
        
        // Wall Pass active indicator
        if (wallPassActive) {
            sf::RectangleShape wallPassInd(sf::Vector2f(80, 15));
            wallPassInd.setPosition(10, 70);
            wallPassInd.setFillColor(sf::Color::Yellow);
            window.draw(wallPassInd);
        }
        
        // Double Score active indicator
        if (doubleScoreActive) {
            sf::RectangleShape doubleScoreInd(sf::Vector2f(80, 15));
            doubleScoreInd.setPosition(10, 95);
            doubleScoreInd.setFillColor(sf::Color::Magenta);
            window.draw(doubleScoreInd);
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake vs Blocks");
    window.setFramerateLimit(60);
    
    GameState game;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.scancode == sf::Keyboard::Scan::Escape)
                    window.close();
                if (event.key.scancode == sf::Keyboard::Scan::R && game.gameOver) {
                    game = GameState();
                    WINDOW_WIDTH = 800;
                    WINDOW_HEIGHT = 600;
                    window.setSize(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT));
                }
                game.handleInput(event.key.scancode);
            }
        }
        
        game.update(0.016f);
        
        // Actualizar tamaño de ventana si cambió
        if (window.getSize().x != WINDOW_WIDTH || window.getSize().y != WINDOW_HEIGHT) {
            window.setSize(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT));
        }
        
        window.clear(sf::Color::Black);
        game.draw(window);
        game.drawUI(window);
        
        window.display();
    }
    
    return 0;
}