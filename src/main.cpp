#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

const int GRID_SIZE = 20;
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
const int PANEL_WIDTH = 280;

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
    float obstacleSpawnDelay = 8.0f;
    float gameTimer = 0;
    int speedLevel = 1;
    float moveCounter = 0;
    int moveDelay = 10;
    int windowExpansionCount = 0;
    bool wallPassActive = false;
    float wallPassTimer = 0;
    bool doubleScoreActive = false;
    float doubleScoreTimer = 0;
    bool magnetActive = false;
    float magnetTimer = 0;
    int baseSpeedBonus = 0;
    
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
        
        // Aumentar velocidad cada 10 segundos + bonificación por manzanas
        speedLevel = 1 + (int)(gameTimer / 10.0f) + (applesEaten / 5);
        moveDelay = 10 - (speedLevel - 1) * 1;
        if (moveDelay < 2) moveDelay = 2;
        
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
        
        if (magnetActive) {
            magnetTimer += deltaTime;
            if (magnetTimer >= 10.0f) {
                magnetActive = false;
                magnetTimer = 0;
            }
        }
        
        direction = nextDirection;
        
        // Si el imán está activo, atraer manzanas hacia la serpiente
        if (magnetActive && !snake.empty()) {
            SnakeSegment head = snake[0];
            for (auto& block : blocks) {
                int blockGridX = block.x / GRID_SIZE;
                int blockGridY = block.y / GRID_SIZE;
                
                // Mover la manzana hacia la cabeza
                if (blockGridX < head.x) blockGridX++;
                else if (blockGridX > head.x) blockGridX--;
                
                if (blockGridY < head.y) blockGridY++;
                else if (blockGridY > head.y) blockGridY--;
                
                block.x = blockGridX * GRID_SIZE;
                block.y = blockGridY * GRID_SIZE;
            }
        }
        
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
                    PowerUpType type = (rand() % 3 == 0) ? WALL_PASS : (rand() % 2 == 0) ? DOUBLE_SCORE : MAGNET;
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
                } else if (it->type == MAGNET) {
                    magnetActive = true;
                    magnetTimer = 0;
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
            } else if (powerUp.type == DOUBLE_SCORE) {
                rect.setFillColor(sf::Color::Magenta);
            } else if (powerUp.type == MAGNET) {
                rect.setFillColor(sf::Color(255, 165, 0)); // Naranja
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
        
        // Línea blanca divisoria entre mapa y panel
        sf::RectangleShape dividerLine(sf::Vector2f(2, WINDOW_HEIGHT));
        dividerLine.setPosition(WINDOW_WIDTH, 0);
        dividerLine.setFillColor(sf::Color::White);
        window.draw(dividerLine);
    }
    
    void drawUI(sf::RenderWindow& window) {
        // Fondo para información en el lado derecho (fuera del mapa)
        sf::RectangleShape infoBg(sf::Vector2f(PANEL_WIDTH - 10, WINDOW_HEIGHT));
        infoBg.setPosition(WINDOW_WIDTH + 5, 0);
        infoBg.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(infoBg);
        
        int panelX = WINDOW_WIDTH + 15;
        int yPos = 10;
        
        // Línea separadora
        sf::RectangleShape separator(sf::Vector2f(PANEL_WIDTH - 20, 1));
        separator.setPosition(panelX, yPos + 25);
        separator.setFillColor(sf::Color::White);
        window.draw(separator);
        
        // SCORE (Puntuación numérica con barra visual)
        // Fondo del score
        sf::RectangleShape scoreBg(sf::Vector2f(PANEL_WIDTH - 20, 25));
        scoreBg.setPosition(panelX, yPos);
        scoreBg.setFillColor(sf::Color(50, 50, 50));
        window.draw(scoreBg);
        
        // Barra de score visual
        int scoreBarWidth = (score / 10) % (PANEL_WIDTH - 20);
        sf::RectangleShape scoreBar(sf::Vector2f(scoreBarWidth, 3));
        scoreBar.setPosition(panelX, yPos + 22);
        scoreBar.setFillColor(sf::Color::Green);
        window.draw(scoreBar);
        
        yPos += 35;
        
        // Indicador de manzanas comidas con número
        // Cuadro rojo para las manzanas
        sf::RectangleShape applesBox(sf::Vector2f(50, 18));
        applesBox.setPosition(panelX, yPos);
        applesBox.setFillColor(sf::Color(100, 0, 0));
        applesBox.setOutlineColor(sf::Color::Red);
        applesBox.setOutlineThickness(2);
        window.draw(applesBox);
        
        // Pequeño círculo rojo para representar manzana
        sf::RectangleShape appleIndicator(sf::Vector2f(8, 8));
        appleIndicator.setPosition(panelX + 5, yPos + 5);
        appleIndicator.setFillColor(sf::Color::Red);
        window.draw(appleIndicator);
        
        // Barra visual de manzanas (número de manzanas consumidas en barras)
        for (int i = 0; i < applesEaten && i < 12; i++) {
            sf::RectangleShape appleBar(sf::Vector2f(3, 10));
            appleBar.setPosition(panelX + 18 + i * 3, yPos + 4);
            appleBar.setFillColor(sf::Color::Red);
            window.draw(appleBar);
        }
        
        yPos += 30;
        
        // Speed level visual
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
        
        // Power-ups con cronómetro
        // Wall Pass active indicator con barra de tiempo
        if (wallPassActive) {
            // Fondo del power-up
            sf::RectangleShape wallPassBg(sf::Vector2f(PANEL_WIDTH - 20, 35));
            wallPassBg.setPosition(panelX, yPos);
            wallPassBg.setFillColor(sf::Color(100, 100, 0));
            window.draw(wallPassBg);
            
            // Barra de duración
            float wallPassProgress = wallPassTimer / 10.0f;
            sf::RectangleShape wallPassBar(sf::Vector2f((PANEL_WIDTH - 20) * (1.0f - wallPassProgress), 5));
            wallPassBar.setPosition(panelX, yPos + 28);
            wallPassBar.setFillColor(sf::Color::Yellow);
            window.draw(wallPassBar);
            
            // Borde del power-up
            sf::RectangleShape wallPassBorder(sf::Vector2f(PANEL_WIDTH - 20, 35));
            wallPassBorder.setPosition(panelX, yPos);
            wallPassBorder.setFillColor(sf::Color::Transparent);
            wallPassBorder.setOutlineColor(sf::Color::Yellow);
            wallPassBorder.setOutlineThickness(2);
            window.draw(wallPassBorder);
            
            yPos += 40;
        }
        
        // Double Score active indicator con barra de tiempo
        if (doubleScoreActive) {
            // Fondo del power-up
            sf::RectangleShape doubleScoreBg(sf::Vector2f(PANEL_WIDTH - 20, 35));
            doubleScoreBg.setPosition(panelX, yPos);
            doubleScoreBg.setFillColor(sf::Color(100, 0, 100));
            window.draw(doubleScoreBg);
            
            // Barra de duración
            float doubleScoreProgress = doubleScoreTimer / 10.0f;
            sf::RectangleShape doubleScoreBar(sf::Vector2f((PANEL_WIDTH - 20) * (1.0f - doubleScoreProgress), 5));
            doubleScoreBar.setPosition(panelX, yPos + 28);
            doubleScoreBar.setFillColor(sf::Color::Magenta);
            window.draw(doubleScoreBar);
            
            // Borde del power-up
            sf::RectangleShape doubleScoreBorder(sf::Vector2f(PANEL_WIDTH - 20, 35));
            doubleScoreBorder.setPosition(panelX, yPos);
            doubleScoreBorder.setFillColor(sf::Color::Transparent);
            doubleScoreBorder.setOutlineColor(sf::Color::Magenta);
            doubleScoreBorder.setOutlineThickness(2);
            window.draw(doubleScoreBorder);
            
            yPos += 40;
        }
        
        // Magnet active indicator con barra de tiempo
        if (magnetActive) {
            // Fondo del power-up
            sf::RectangleShape magnetBg(sf::Vector2f(PANEL_WIDTH - 20, 35));
            magnetBg.setPosition(panelX, yPos);
            magnetBg.setFillColor(sf::Color(165, 100, 0)); // Naranja oscuro
            window.draw(magnetBg);
            
            // Barra de duración
            float magnetProgress = magnetTimer / 10.0f;
            sf::RectangleShape magnetBar(sf::Vector2f((PANEL_WIDTH - 20) * (1.0f - magnetProgress), 5));
            magnetBar.setPosition(panelX, yPos + 28);
            magnetBar.setFillColor(sf::Color(255, 165, 0)); // Naranja
            window.draw(magnetBar);
            
            // Borde del power-up
            sf::RectangleShape magnetBorder(sf::Vector2f(PANEL_WIDTH - 20, 35));
            magnetBorder.setPosition(panelX, yPos);
            magnetBorder.setFillColor(sf::Color::Transparent);
            magnetBorder.setOutlineColor(sf::Color(255, 165, 0)); // Naranja
            magnetBorder.setOutlineThickness(2);
            window.draw(magnetBorder);
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH + PANEL_WIDTH, WINDOW_HEIGHT), "Snake vs Blocks");
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
                    window.setSize(sf::Vector2u(WINDOW_WIDTH + PANEL_WIDTH, WINDOW_HEIGHT));
                }
                game.handleInput(event.key.scancode);
            }
        }
        
        game.update(0.016f);
        
        // Actualizar tamaño de ventana si cambió
        if (window.getSize().x != WINDOW_WIDTH + PANEL_WIDTH || window.getSize().y != WINDOW_HEIGHT) {
            window.setSize(sf::Vector2u(WINDOW_WIDTH + PANEL_WIDTH, WINDOW_HEIGHT));
        }
        
        window.clear(sf::Color::Black);
        game.draw(window);
        game.drawUI(window);
        
        window.display();
    }
    
    return 0;
}