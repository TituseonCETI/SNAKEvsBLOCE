// ============================================================
// SNAKE vs BLOCKS - Juego en C++ con SFML 3.0.2
// ============================================================
// Un juego de serpiente mejorado con obstáculos, power-ups,
// y un sistema de puntuación progresivo.
// ============================================================

// ============================================================
// LIBRERÍAS INCLUIDAS
// ============================================================
#include <SFML/Graphics.hpp>  // Gráficos y renderizado
#include <SFML/Audio.hpp>
#include <iostream>           // Para debug output
#include <fstream>            // Para escribir en archivos
#include <vector>             // Contenedor dinámico
#include <cstdlib>            // Números aleatorios
#include <ctime>              // Para seed del RNG
#include <string>             // Manejo de strings
#include <sstream>            // Conversión a strings

// ============================================================
// CONSTANTES DE CONFIGURACIÓN
// ============================================================

// Tamaño de cada celda del grid (en píxeles)
const int GRID_SIZE = 20;

// Resoluciones base (sin panel lateral)
const int BASE_WINDOW_WIDTH = 800;
const int BASE_WINDOW_HEIGHT = 600;

// Ancho del panel lateral con información
const int PANEL_WIDTH = 280;

// ============================================================
// VARIABLES GLOBALES DE PANTALLA
// ============================================================

// Dimensiones de la ventana de juego (cambian según resolución)
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

// Ancho total (juego + panel)
int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

// Factores de escala para adaptarse a diferentes resoluciones
float SCALE_X = 1.0f;
float SCALE_Y = 1.0f;

// Textura global para fondos
sf::Texture tex;

// ========== MÚSICA Y SONIDO ==========
// Puntero global para la música de fondo
sf::Music* backgroundMusic = nullptr;

// ============================================================
// ENUMERACIONES - Tipos de estados y power-ups
// ============================================================

// Estados principales del juego
enum GameState_Type {
    MENU,        // Menú principal
    PLAYING,     // Jugando
    RULES,       // Pantalla de reglas
    GAME_OVER    // Pantalla de fin de juego
};

// Tipos de power-ups disponibles
enum PowerUpType {
    WALL_PASS,           // Permite atravesar paredes (10s)
    DOUBLE_SCORE,        // Duplica puntos (10s)
    MAGNET,              // Atrae manzanas hacia la serpiente (10s)
    OBSTACLE_DESTROYER   // Destruye todos los obstáculos (instantáneo)
};

// ============================================================
// ESTRUCTURAS DE DATOS
// ============================================================

// Representa un segmento de la serpiente
struct SnakeSegment {
    int x, y;  // Posición en el grid
    
    SnakeSegment(int x = 0, int y = 0) : x(x), y(y) {}
    
    // Compara si dos segmentos están en la misma posición
    bool operator==(const SnakeSegment& other) const {
        return x == other.x && y == other.y;
    }
};

// Representa una manzana/bloque para comer
struct Block {
    int x, y;           // Posición en píxeles
    int width, height;  // Dimensiones
    
    Block(int x, int y, int width = GRID_SIZE, int height = GRID_SIZE) 
        : x(x), y(y), width(width), height(height) {}
};

// Representa un power-up especial
struct PowerUp {
    int x, y;           // Posición en píxeles
    PowerUpType type;   // Tipo de poder
    int width, height;  // Dimensiones
    
    PowerUp(int x, int y, PowerUpType type) 
        : x(x), y(y), type(type), width(GRID_SIZE), height(GRID_SIZE) {}
};

// Representa un obstáculo en el mapa
struct Obstacle {
    int x, y;           // Posición en píxeles
    int width, height;  // Dimensiones
    
    Obstacle(int x, int y) 
        : x(x), y(y), width(GRID_SIZE), height(GRID_SIZE) {}
};

// ============================================================
// FUNCIÓN DE CÁLCULO DE ESCALA
// ============================================================

// Calcula los factores de escala según la resolución de pantalla
void calculateScaling() {
    // Ancho total = ancho del juego + ancho del panel
    SCREEN_WIDTH = WINDOW_WIDTH + PANEL_WIDTH;
    SCREEN_HEIGHT = WINDOW_HEIGHT;
    
    // Factores de escala para adaptarse a diferentes resoluciones
    SCALE_X = (float)WINDOW_WIDTH / BASE_WINDOW_WIDTH;
    SCALE_Y = (float)WINDOW_HEIGHT / BASE_WINDOW_HEIGHT;
}

// ============================================================
// CLASE: MENÚ PRINCIPAL
// ============================================================

class Menu {
public:
    int selectedOption = 0;  // Opción seleccionada (0=Iniciar, 1=Reglas, 2=Salir)
    
    // ========== TEXTURAS Y SPRITES DE BOTONES ==========
    sf::Texture texInitButton;      // Textura del botón "INICIAR JUEGO"
    sf::Sprite sprInitButton;       // Sprite del botón "INICIAR JUEGO"
    
    sf::Texture texRulesButton;     // Textura del botón "REGLAS"
    sf::Sprite sprRulesButton;      // Sprite del botón "REGLAS"
    
    sf::Texture texExitButton;      // Textura del botón "SALIR"
    sf::Sprite sprExitButton;       // Sprite del botón "SALIR"
    
    // ========== CONSTRUCTOR ==========
    // Carga las texturas y configura los sprites
    Menu() {
        // Cargar texturas desde archivos
        texInitButton.loadFromFile("assets/images/Boton.iniciar.png");
        texRulesButton.loadFromFile("assets/images/Boton.reglas.png");
        texExitButton.loadFromFile("assets/images/Boton.salir.png");
        
        // Asignar texturas a sprites
        sprInitButton.setTexture(texInitButton);
        sprRulesButton.setTexture(texRulesButton);
        sprExitButton.setTexture(texExitButton);
        
        // Posicionar los botones en el menú (centrados horizontalmente, espaciados verticalmente)
        // El posicionamiento se hace en el método draw() para adaptarse mejor a la pantalla
    }
    
    // Maneja entrada del usuario en el menú
    void handleInput(sf::Keyboard::Scancode key) {
        // Flecha arriba: selecciona opción anterior
        if (key == sf::Keyboard::Scan::Up && selectedOption > 0) {
            selectedOption--;
        } 
        // Flecha abajo: selecciona opción siguiente
        else if (key == sf::Keyboard::Scan::Down && selectedOption < 2) {
            selectedOption++;
        }
    }
    
    // Dibuja el menú principal en pantalla
    void draw(sf::RenderWindow& window) {
        // Limpia la pantalla
        window.clear(sf::Color::Black);
        
        // Dibuja el fondo con la textura cargada
        sf::RectangleShape background(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
        background.setPosition(0, 0);
        background.setTexture(&tex);
        window.draw(background);
        
        // Caja oscura sobre el título para mejor legibilidad
        sf::RectangleShape titleBg(sf::Vector2f(SCREEN_WIDTH, 120));
        titleBg.setPosition(0, 40);
        titleBg.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(titleBg);
        
        // ========== POSICIONAMIENTO Y DIBUJO DE BOTONES ==========
        // Obtener dimensiones de los botones para centrarlos
        float buttonWidth = sprInitButton.getLocalBounds().width;
        float buttonHeight = sprInitButton.getLocalBounds().height;
        
        // Array de sprites para facilitar el manejo
        std::vector<sf::Sprite*> sprites = {&sprInitButton, &sprRulesButton, &sprExitButton};
        
        // Dibuja cada botón del menú
        for (int i = 0; i < 3; i++) {
            // Posicionar el botón (centrado horizontalmente)
            float posX = (SCREEN_WIDTH / 0.5f) - (buttonWidth * 0.25f);
            float posY = 100 + i * 50;  // Espaciado vertical entre botones
            
            sprites[i]->setPosition(posX, posY);
            
            // Si está seleccionado, aplicar escala y color de resaltado
            if (i == selectedOption) {
                // Resaltar con escala aumentada
                sprites[i]->setScale(0.0f, 0.0f);
                sprites[i]->setColor(sf::Color::White);
                
                // Dibujar un borde blanco de resaltado alrededor del botón
                sf::RectangleShape highlightBorder(sf::Vector2f(buttonWidth * 0.10f, buttonHeight * 0.10f));
                highlightBorder.setPosition(posX - (buttonWidth * 0.025f), posY - (buttonHeight * 0.025f));
                highlightBorder.setFillColor(sf::Color::Transparent);
                highlightBorder.setOutlineColor(sf::Color::White);
                highlightBorder.setOutlineThickness(3);
                window.draw(highlightBorder);
            } 
            // Si no está seleccionado, mostrar con opacidad normal
            else {
                sprites[i]->setScale(0.0f, 0.0f);
                sprites[i]->setColor(sf::Color(200, 200, 200));  // Ligeramente oscurecido
            }
            
            // Dibujar el sprite del botón
            window.draw(*sprites[i]);
        }
    }
    
    // Retorna la opción seleccionada
    int getSelectedOption() {
        return selectedOption;
    }
};

// ============================================================
// CLASE: PANTALLA DE REGLAS
// ============================================================

class Rules {
public:
    // Dibuja la pantalla de reglas
    void draw(sf::RenderWindow& window) {
        // Limpia la pantalla
        window.clear(sf::Color::Black);
        
        // Título oscuro
        sf::RectangleShape titleBg(sf::Vector2f(SCREEN_WIDTH, 80));
        titleBg.setPosition(0, 20);
        titleBg.setFillColor(sf::Color(50, 50, 50));
        window.draw(titleBg);
        
        // Caja principal para las reglas
        sf::RectangleShape rulesBg(sf::Vector2f(SCREEN_WIDTH - 40, SCREEN_HEIGHT - 120));
        rulesBg.setPosition(20, 120);
        rulesBg.setFillColor(sf::Color(30, 30, 30));
        rulesBg.setOutlineColor(sf::Color::White);
        rulesBg.setOutlineThickness(2);
        window.draw(rulesBg);
        
        // Caja de instrucción de salida (inferior)
        sf::RectangleShape exitBg(sf::Vector2f(SCREEN_WIDTH - 40, 50));
        exitBg.setPosition(20, SCREEN_HEIGHT - 60);
        exitBg.setFillColor(sf::Color(50, 50, 0));
        window.draw(exitBg);
    }
};

// ==========================================
// CLASE GameState
// ==========================================
// Gestiona el estado completo del juego mientras está en ejecución.
// Incluye:
// - Movimiento de la serpiente
// - Generación de bloques, obstáculos y power-ups
// - Detección de colisiones
// - Sistema de puntuación
// - Manejo de power-ups activos
class GameState {
public:
    // ========== DATOS DEL JUEGO ==========
    std::vector<SnakeSegment> snake;        // Segmentos que forman el cuerpo de la serpiente
    std::vector<Block> blocks;              // Bloques/manzanas a comer
    std::vector<PowerUp> powerUps;          // Power-ups en el mapa
    std::vector<Obstacle> obstacles;        // Obstáculos que causan game over
    
    // ========== PUNTUACIÓN Y ESTADO GENERAL ==========
    int score = 0;                          // Puntos acumulados (10 por manzana, 20 si double score activo)
    int applesEaten = 0;                    // Contador de manzanas comidas (afecta velocidad)
    bool gameOver = false;                  // Flag de fin de juego
    
    // ========== MOVIMIENTO Y DIRECCIÓN ==========
    int direction = 1;                      // Dirección actual (0=arriba, 1=derecha, 2=abajo, 3=izquierda)
    int nextDirection = 1;                  // Siguiente dirección (se aplica en el siguiente frame)
    float moveCounter = 0;                  // Contador para controlar velocidad (incrementa cada frame)
    int moveDelay = 10;                     // Delay entre movimientos (afectado por velocidad)
    
    // ========== SPAWN DE BLOQUES ==========
    float blockSpawnTimer = 0;              // Timer para spawn de manzanas
    float blockSpawnDelay = 5.0f;           // Intervalo entre spawns (5 segundos)
    
    // ========== SPAWN DE POWER-UPS ==========
    float powerUpSpawnTimer = 0;            // Timer para spawn de power-ups
    float powerUpSpawnDelay = 15.0f;        // Intervalo entre spawns (15 segundos)
    
    // ========== SPAWN DE OBSTÁCULOS ==========
    float obstacleSpawnTimer = 0;           // Timer para spawn de obstáculos
    float obstacleSpawnDelay = 4.0f;        // Intervalo entre spawns (4 segundos)
    
    // ========== SPAWN DE OBSTACLE DESTROYER ==========
    float obstacleDestroyerSpawnTimer = 0; // Timer para spawn de destructor
    float obstacleDestroyerSpawnDelay = 30.0f; // Aparece cada 30 segundos (solo si 15+ obstáculos)
    
    // ========== POWER-UPS ACTIVOS ==========
    bool wallPassActive = false;            // Si verdadero, la serpiente puede atravesar paredes
    float wallPassTimer = 0;                // Tiempo restante del power-up WALL_PASS
    
    bool doubleScoreActive = false;         // Si verdadero, cada manzana vale 20 puntos (en lugar de 10)
    float doubleScoreTimer = 0;             // Tiempo restante del power-up DOUBLE_SCORE
    
    bool magnetActive = false;              // Si verdadero, las manzanas se atraen hacia la serpiente
    float magnetTimer = 0;                  // Tiempo restante del power-up MAGNET
    
    // ========== OTROS ==========
    float gameTimer = 0;                    // Timer global del juego
    int speedLevel = 1;                     // Nivel de velocidad (aumenta con manzanas comidas)
    
    // ========== CONSTRUCTOR ==========
    // Inicializa el juego con la serpiente en el centro de la pantalla
    GameState() {
        snake.push_back(SnakeSegment(WINDOW_WIDTH / (2 * GRID_SIZE), WINDOW_HEIGHT / (2 * GRID_SIZE)));
    }
    
    // ========== MANEJO DE ENTRADA ==========
    // Actualiza la dirección de movimiento según la entrada del usuario
    // Evita que la serpiente se doble sobre sí misma (no puede ir en dirección opuesta)
    void handleInput(sf::Keyboard::Scancode key) {
        if (key == sf::Keyboard::Scan::Up && direction != 2) nextDirection = 0;        // Arriba (no desde abajo)
        else if (key == sf::Keyboard::Scan::Right && direction != 3) nextDirection = 1; // Derecha (no desde izquierda)
        else if (key == sf::Keyboard::Scan::Down && direction != 0) nextDirection = 2;  // Abajo (no desde arriba)
        else if (key == sf::Keyboard::Scan::Left && direction != 1) nextDirection = 3;  // Izquierda (no desde derecha)
    }
    // ========== ACTUALIZACIÓN DEL JUEGO ==========
    // Se ejecuta cada frame (60 veces por segundo)
    // Maneja: power-ups, timers de spawn, movimiento de la serpiente, colisiones
    void update(float deltaTime) {
        if (gameOver) return;  // Si el juego terminó, no actualizar nada
        
        gameTimer += deltaTime;  // Incrementar timer global del juego
        
        // ========== CÁLCULO DE VELOCIDAD ==========
        // Cada 10 manzanas comidas, la serpiente se mueve más rápido
        speedLevel = 1 + (applesEaten / 10);
        // Cada nivel de velocidad reduce el delay en 0.5 unidades
        moveDelay = 10 - (speedLevel - 1) * 0.5f;
        if (moveDelay < 2) moveDelay = 2;  // Límite mínimo de velocidad
        
        // ========== POWER-UP: WALL PASS ==========
        // Permite a la serpiente atravesar las paredes durante 10 segundos
        if (wallPassActive) {
            wallPassTimer += deltaTime;
            if (wallPassTimer >= 10.0f) {
                wallPassActive = false;
                wallPassTimer = 0;
            }
        }
        
        // ========== POWER-UP: DOUBLE SCORE ==========
        // Duplica los puntos obtenidos (20 por manzana en lugar de 10) por 10 segundos
        if (doubleScoreActive) {
            doubleScoreTimer += deltaTime;
            if (doubleScoreTimer >= 10.0f) {
                doubleScoreActive = false;
                doubleScoreTimer = 0;
            }
        }
        
        // ========== POWER-UP: MAGNET ==========
        // Las manzanas se atraen hacia la serpiente durante 10 segundos
        if (magnetActive) {
            magnetTimer += deltaTime;
            if (magnetTimer >= 10.0f) {
                magnetActive = false;
                magnetTimer = 0;
            }
        }
        
        // Actualizar dirección a la siguiente entrada del usuario
        direction = nextDirection;
        
        // ========== MAGNET LOGIC ==========
        // Si el power-up MAGNET está activo, atraer los bloques hacia la cabeza
        if (magnetActive && !snake.empty()) {
            SnakeSegment head = snake[0];  // Posición de la cabeza
            std::vector<int> blocksToRemove;
            for (int i = 0; i < blocks.size(); i++) {
                auto& block = blocks[i];
                int blockGridX = block.x / GRID_SIZE;
                int blockGridY = block.y / GRID_SIZE;

                // Atraer el bloque hacia la cabeza (mover en dirección X)
                if (blockGridX < head.x) blockGridX++;
                else if (blockGridX > head.x) blockGridX--;

                if (blockGridY < head.y) blockGridY++;
                else if (blockGridY > head.y) blockGridY--;

                block.x = blockGridX * GRID_SIZE;
                block.y = blockGridY * GRID_SIZE;

                // Si el bloque llega a la cabeza con MAGNET, comerlo automáticamente
                if (block.x == head.x * GRID_SIZE && block.y == head.y * GRID_SIZE) {
                    int points = doubleScoreActive ? 20 : 10;
                    score += points;
                    applesEaten++;
                    blocksToRemove.push_back(i);
                }
            }
            // Remover bloques comidos (en orden inverso para evitar cambios de índice)
            for (int i = blocksToRemove.size() - 1; i >= 0; i--) {
                blocks.erase(blocks.begin() + blocksToRemove[i]);
            }
        }
        
        // Incrementar el contador de movimiento según el nivel de velocidad
        moveCounter += speedLevel;
        
        // ========== SPAWN DE ELEMENTOS (INDEPENDIENTE DEL MOVIMIENTO) ==========
        // IMPORTANTE: Los spawns se ejecutan cada frame (60 veces/segundo)
        // NO afectan la velocidad de movimiento de la serpiente
        
        // SPAWN: OBSTACLE_DESTROYER (Power-up blanco que destruye todos los obstáculos)
        // Solo aparece cuando hay 15 o más obstáculos, cada 30 segundos
        if (obstacles.size() >= 15) {
            obstacleDestroyerSpawnTimer += 0.016f;  // Incrementar cada frame
            if (obstacleDestroyerSpawnTimer >= obstacleDestroyerSpawnDelay) {
                int randomX = rand() % (WINDOW_WIDTH / GRID_SIZE);
                int randomY = rand() % (WINDOW_HEIGHT / GRID_SIZE);
                bool validPos = true;  // Flag para validar que la posición no esté ocupada
                
                // Verificar que no colisione con la serpiente
                for (const auto& segment : snake) {
                    if (segment.x == randomX && segment.y == randomY) {
                        validPos = false;
                        break;
                    }
                }
                
                // Si la posición es válida, crear el power-up
                if (validPos) {
                    powerUps.push_back(PowerUp(randomX * GRID_SIZE, randomY * GRID_SIZE, OBSTACLE_DESTROYER));
                }
                // Resetear el timer después de spawning
                obstacleDestroyerSpawnTimer = 0;
            }
        } else {
            // Si hay menos de 15 obstáculos, resetear el timer
            obstacleDestroyerSpawnTimer = 0;
        }
        
        // SPAWN: POWER-UPS NORMALES (cada 15 segundos)
        // Puede generar: WALL_PASS (33%), DOUBLE_SCORE (33%), o MAGNET (33%)
        powerUpSpawnTimer += 0.016f;
        if (powerUpSpawnTimer >= powerUpSpawnDelay) {
            int randomX = rand() % (WINDOW_WIDTH / GRID_SIZE);
            int randomY = rand() % (WINDOW_HEIGHT / GRID_SIZE);
            bool validPos = true;
            
            // Verificar que la posición no esté ocupada por la serpiente
            for (const auto& segment : snake) {
                if (segment.x == randomX && segment.y == randomY) {
                    validPos = false;
                    break;
                }
            }
            
            // Si la posición es válida, seleccionar tipo aleatorio y crear power-up
            if (validPos) {
                // Seleccionar tipo: 1/3 para cada poder
                PowerUpType type = (rand() % 3 == 0) ? WALL_PASS : (rand() % 2 == 0) ? DOUBLE_SCORE : MAGNET;
                powerUps.push_back(PowerUp(randomX * GRID_SIZE, randomY * GRID_SIZE, type));
            }
            // Resetear el timer
            powerUpSpawnTimer = 0;
        }
        
        // SPAWN: OBSTÁCULOS (cada 8 segundos)
        // Los obstáculos causan game over si colisionan con la serpiente
        // Máximo 20 obstáculos en pantalla
        obstacleSpawnTimer += 0.016f;
        if (obstacleSpawnTimer >= obstacleSpawnDelay) {
            int randomX = rand() % (WINDOW_WIDTH / GRID_SIZE);
            int randomY = rand() % (WINDOW_HEIGHT / GRID_SIZE);
            bool validPos = true;
            
            // Verificar que no esté en la serpiente
            for (const auto& segment : snake) {
                if (segment.x == randomX && segment.y == randomY) {
                    validPos = false;
                    break;
                }
            }
            
            // Solo crear si la posición es válida y no hay demasiados obstáculos
            if (validPos && obstacles.size() < 30) {
                obstacles.push_back(Obstacle(randomX * GRID_SIZE, randomY * GRID_SIZE));
            }
            // Resetear el timer
            obstacleSpawnTimer = 0;
        }
        
        // SPAWN: MANZANAS (cada 1 segundo)
        // Las manzanas aumentan puntuación y velocidad
        blockSpawnTimer += 0.016f;
        if (blockSpawnTimer >= blockSpawnDelay) {
            int randomX = rand() % (WINDOW_WIDTH / GRID_SIZE);
            int randomY = rand() % (WINDOW_HEIGHT / GRID_SIZE);
            
            bool onSnake = false;
            // Verificar que no esté en la serpiente
            for (const auto& segment : snake) {
                if (segment.x == randomX && segment.y == randomY) {
                    onSnake = true;
                    break;
                }
            }
            
            // Si no está en la serpiente, crear la manzana
            if (!onSnake) {
                blocks.push_back(Block(randomX * GRID_SIZE, randomY * GRID_SIZE));
            }
            
            // Resetear el timer
            blockSpawnTimer = 0;
        }
        
        // ========== MOVIMIENTO DE LA SERPIENTE ==========
        // IMPORTANTE: El movimiento SOLO ocurre cuando moveCounter >= moveDelay
        // Los spawns (arriba) ocurren independientemente cada frame
        // Esto permite que los power-ups aparezcan correctamente sin afectar velocidad
        
        if (moveCounter < moveDelay) {
            return;  // Si no es tiempo de mover, salir de la función
        }
        moveCounter = 0;  // Resetear contador para próximo movimiento
        
        // ========== CÁLCULO DE NUEVA POSICIÓN DE CABEZA ==========
        // Crear nueva cabeza basada en dirección actual
        SnakeSegment head = snake[0];  // Copiar posición actual
        if (direction == 0) head.y--;           // Arriba: decrementar Y
        else if (direction == 1) head.x++;      // Derecha: incrementar X
        else if (direction == 2) head.y++;      // Abajo: incrementar Y
        else if (direction == 3) head.x--;      // Izquierda: decrementar X
        
        // ========== COLISIÓN: PAREDES ==========
        // Verificar si la cabeza sale de los límites de pantalla
        if (!wallPassActive) {
            // Sin power-up: colisionar con paredes causa game over
            if (head.x < 0 || head.x >= WINDOW_WIDTH / GRID_SIZE ||
                head.y < 0 || head.y >= WINDOW_HEIGHT / GRID_SIZE) {
                gameOver = true;
                return;
            }
        } else {
            // Con WALL_PASS: envolver a la posición opuesta (efecto de túnel)
            if (head.x < 0) head.x = WINDOW_WIDTH / GRID_SIZE - 1;
            else if (head.x >= WINDOW_WIDTH / GRID_SIZE) head.x = 0;
            if (head.y < 0) head.y = WINDOW_HEIGHT / GRID_SIZE - 1;
            else if (head.y >= WINDOW_HEIGHT / GRID_SIZE) head.y = 0;
        }
        
        // ========== COLISIÓN: AUTO-COLISIÓN (SERPIENTE CONSIGO MISMA) ==========
        // Verificar si la cabeza colisiona con algún segmento del cuerpo
        for (const auto& segment : snake) {
            if (head == segment) {
                gameOver = true;
                return;
            }
        }
        
        // ========== COLISIÓN: OBSTÁCULOS ==========
        // Verificar si la cabeza colisiona con algún obstáculo
        for (const auto& obstacle : obstacles) {
            if (head.x * GRID_SIZE == obstacle.x && head.y * GRID_SIZE == obstacle.y) {
                gameOver = true;
                return;
            }
        }
        
        // ========== MOVIMIENTO: INSERTAR CABEZA ==========
        // Agregar la nueva cabeza al inicio de la lista
        snake.insert(snake.begin(), head);
        
        // ========== COMER: BLOQUES/MANZANAS ==========
        // Verificar si la cabeza está en la posición de alguna manzana
        bool ateBlock = false;  // Flag para saber si comió algo (decide si crece)
        for (auto it = blocks.begin(); it != blocks.end(); ++it) {
            if (head.x * GRID_SIZE == it->x && head.y * GRID_SIZE == it->y) {
                // Calcular puntos (double si power-up activo)
                int points = doubleScoreActive ? 20 : 10;
                score += points;
                applesEaten++;  // Incrementar contador (afecta velocidad)
                blocks.erase(it);  // Remover la manzana
                ateBlock = true;
                break;
            }
        }
        
        // ========== COMER: POWER-UPS ==========
        // Verificar si la cabeza está en la posición de algún power-up
        for (auto it = powerUps.begin(); it != powerUps.end(); ++it) {
            if (head.x * GRID_SIZE == it->x && head.y * GRID_SIZE == it->y) {
                // Aplicar efecto según el tipo de power-up
                if (it->type == WALL_PASS) {
                    // WALL_PASS: Permite atravesar paredes por 10 segundos
                    wallPassActive = true;
                    wallPassTimer = 0;
                } else if (it->type == DOUBLE_SCORE) {
                    // DOUBLE_SCORE: Manzanas valen el doble (20 en lugar de 10) por 10 segundos
                    doubleScoreActive = true;
                    doubleScoreTimer = 0;
                } else if (it->type == MAGNET) {
                    // MAGNET: Atraer manzanas hacia la serpiente por 10 segundos
                    magnetActive = true;
                    magnetTimer = 0;
                } else if (it->type == OBSTACLE_DESTROYER) {
                    // OBSTACLE_DESTROYER: Eliminar TODOS los obstáculos y ganar 50 bonus
                    obstacles.clear();  // Limpiar lista de obstáculos
                    score += 50;  // Bonus de puntos
                }
                // Remover el power-up consumido
                powerUps.erase(it);
                break;
            }
        }
        
        // ========== CRECIMIENTO/ENCOGIMIENTO DE LA SERPIENTE ==========
        // Si NO comió nada, remover el último segmento (la serpiente no crece)
        // Si comió, mantiene el segmento extra (la serpiente crece)
        if (!ateBlock && snake.size() > 1) {
            snake.pop_back();  // Remover cola
        }
    }
    
    // ========== DIBUJAR JUEGO ==========
    // Renderiza todos los elementos visuales en la ventana
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
            } else if (powerUp.type == OBSTACLE_DESTROYER) {
                rect.setFillColor(sf::Color::White);  // BLANCO
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
    
    // ========== TEXTURAS Y SPRITES DE BOTONES ==========
    sf::Texture texRestartButton;   // Textura del botón "REINICIAR"
    sf::Sprite sprRestartButton;    // Sprite del botón "REINICIAR"
    
    sf::Texture texExitButton;      // Textura del botón "SALIR"
    sf::Sprite sprExitButton;       // Sprite del botón "SALIR"
    
    // ========== CONSTRUCTOR ==========
    // Carga las texturas y configura los sprites
    GameOverMenu() {
        // Cargar texturas desde archivos
        texRestartButton.loadFromFile("assets/images/Boton.iniciar.png");
        texExitButton.loadFromFile("assets/images/Boton.salir.png");
        
        // Asignar texturas a sprites
        sprRestartButton.setTexture(texRestartButton);
        sprExitButton.setTexture(texExitButton);
    }
    
    void show(int score, int applesEaten) {
        // Mostrar la pantalla de game over con los scores finales
        isVisible = true;
        finalScore = score;
        finalApplesEaten = applesEaten;
        selectedOption = 0;
    }
    
    // Maneja entrada del usuario en la pantalla de game over
    void handleInput(sf::Keyboard::Scancode key) {
        // Flecha izquierda/arriba: selecciona "REINICIAR" (opción 0)
        if (key == sf::Keyboard::Scan::Left || key == sf::Keyboard::Scan::Up) {
            selectedOption = 0;
        } 
        // Flecha derecha/abajo: selecciona "SALIR" (opción 1)
        else if (key == sf::Keyboard::Scan::Right || key == sf::Keyboard::Scan::Down) {
            selectedOption = 1;
        }
    }
    
    // Dibuja la pantalla de game over con información del juego finalizado
    void draw(sf::RenderWindow& window) {
        if (!isVisible) return;  // No dibujar si no está visible
        
        // Fondo oscuro semitransparente (oscurece el juego de fondo)
        sf::RectangleShape background(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
        background.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(background);
        
        // Cuadro principal del menú de game over
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
        
        // Título "GAME OVER" (fondo rojo)
        sf::RectangleShape titleBg(sf::Vector2f(menuWidth, 60));
        titleBg.setPosition(menuX, menuY);
        titleBg.setFillColor(sf::Color::Red);
        window.draw(titleBg);
        
        // Sección de puntuación
        int yOffset = menuY + 80;
        
        // Label "SCORE:"
        sf::RectangleShape scoreLabel(sf::Vector2f(150, 20));
        scoreLabel.setPosition(menuX + 20, yOffset);
        scoreLabel.setFillColor(sf::Color::Green);
        window.draw(scoreLabel);
        
        std::ostringstream scoreStr;
        scoreStr << "SCORE: " << finalScore;
        
        // Línea visual para el score
        yOffset += 30;
        sf::RectangleShape scoreLine(sf::Vector2f(300, 2));
        scoreLine.setPosition(menuX + 100, yOffset);
        scoreLine.setFillColor(sf::Color::Green);
        window.draw(scoreLine);
        
        // Sección de manzanas comidas
        yOffset += 40;
        sf::RectangleShape applesLabel(sf::Vector2f(200, 20));
        applesLabel.setPosition(menuX + 20, yOffset);
        applesLabel.setFillColor(sf::Color::Yellow);
        window.draw(applesLabel);
        
        std::ostringstream applesStr;
        applesStr << "MANZANAS: " << finalApplesEaten;
        
        // Línea visual para las manzanas
        yOffset += 30;
        sf::RectangleShape applesLine(sf::Vector2f(300, 2));
        applesLine.setPosition(menuX + 100, yOffset);
        applesLine.setFillColor(sf::Color::Yellow);
        window.draw(applesLine);
        
        // ========== POSICIONAMIENTO Y DIBUJO DE BOTONES ==========
        yOffset += 50;
        
        // Obtener dimensiones de los botones para centrarlos
        float buttonWidth = sprRestartButton.getLocalBounds().width;
        float buttonHeight = sprRestartButton.getLocalBounds().height;
        
        // Array de sprites para facilitar el manejo
        std::vector<sf::Sprite*> sprites = {&sprRestartButton, &sprExitButton};
        
        // Posicionar y dibujar los botones (lado a lado)
        for (int i = 0; i < 2; i++) {
            // Botones lado a lado (izquierda y derecha)
            float posX = menuX + 50 + i * 250;
            float posY = yOffset;
            
            sprites[i]->setPosition(posX, posY);
            
            // Si está seleccionado, aplicar efecto de resaltado
            if (i == selectedOption) {
                // Resaltar con escala aumentada
                sprites[i]->setScale(1.1f, 1.1f);
                sprites[i]->setColor(sf::Color::White);
                
                // Dibujar un borde blanco de resaltado alrededor del botón
                sf::RectangleShape highlightBorder(sf::Vector2f(buttonWidth * 1.1f, buttonHeight * 1.1f));
                highlightBorder.setPosition(posX - (buttonWidth * 0.05f), posY - (buttonHeight * 0.05f));
                highlightBorder.setFillColor(sf::Color::Transparent);
                highlightBorder.setOutlineColor(sf::Color::White);
                highlightBorder.setOutlineThickness(3);
                window.draw(highlightBorder);
            } 
            // Si no está seleccionado, mostrar con opacidad normal
            else {
                sprites[i]->setScale(1.0f, 1.0f);
                sprites[i]->setColor(sf::Color(200, 200, 200));  // Ligeramente oscurecido
            }
            
            // Dibujar el sprite del botón
            window.draw(*sprites[i]);
        }
    }
};

int main() {
    try {
        srand(static_cast<unsigned>(time(0)));
        
        calculateScaling();
        
        sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Snake vs Blocks");
        if (!window.isOpen()) {
            std::cerr << "Error: No se pudo crear la ventana" << std::endl;
            return 1;
        }
        window.setFramerateLimit(60);
        
        // Cargar textura de fondo del menú
        if (!tex.loadFromFile("C:/Users/Bienvenido/Desktop/SNAKEvsBLOCE/assets/images/Menu.Fondo.png.png")) {
            std::cerr << "Error: No se pudo cargar la textura del menú" << std::endl;
            return 1;
        }
        
        // ========== CARGAR MÚSICA DE FONDO ==========
        // Crear música en el heap y cargar MP3
        backgroundMusic = new sf::Music();
        if (!backgroundMusic->openFromFile("assets/music/vlog-beat-background-349853.ogg")) {
            std::cerr << "Error: No se pudo cargar la música de fondo" << std::endl;
        } else {
            backgroundMusic->setLoop(true);
            backgroundMusic->setVolume(50.0f);
            backgroundMusic->play();
        }
    
    // ========== INICIALIZACIÓN DE ESTADOS Y OBJETOS ==========
    GameState_Type gameState = MENU;  // Estado inicial es el menú
    Menu menu;                        // Instancia del menú principal
    Rules rules;                      // Instancia de la pantalla de reglas
    GameState game;                   // Instancia del juego
    GameOverMenu gameOverMenu;        // Instancia del menú de game over
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                // ========== TECLA ESC: Regresar al menú ==========
                if (event.key.scancode == sf::Keyboard::Scan::Escape) {
                    if (gameState == PLAYING || gameState == GAME_OVER) {
                        gameState = MENU;
                        game = GameState();
                        gameOverMenu.isVisible = false;
                    } else if (gameState == RULES) {
                        gameState = MENU;
                    } else {
                        window.close();
                    }
                }
                
                // ========== MANEJO DE ENTRADA POR ESTADO ==========
                if (gameState == MENU) {
                    menu.handleInput(event.key.scancode);
                    
                    if (event.key.scancode == sf::Keyboard::Scan::Enter) {
                        int option = menu.getSelectedOption();
                        if (option == 0) {
                            // Opción: INICIAR JUEGO
                            gameState = PLAYING;
                            game = GameState();
                            gameOverMenu.isVisible = false;
                        } else if (option == 1) {
                            // Opción: REGLAS
                            gameState = RULES;
                        } else if (option == 2) {
                            // Opción: SALIR
                            window.close();
                        }
                    }
                } else if (gameState == RULES) {
                    if (event.key.scancode == sf::Keyboard::Scan::Enter) {
                        gameState = MENU;
                        menu.selectedOption = 0;
                    }
                } else if (gameState == PLAYING) {
                    // ========== MANEJO DE ENTRADA EN JUEGO ==========
                    if (game.gameOver) {
                        // Si el juego terminó, mostrar pantalla de game over
                        if (event.key.scancode == sf::Keyboard::Scan::Enter) {
                            int option = gameOverMenu.selectedOption;
                            if (option == 0) {
                                // Reiniciar juego
                                gameState = PLAYING;
                                game = GameState();
                                gameOverMenu.isVisible = false;
                            } else if (option == 1) {
                                // Salir al menú
                                gameState = MENU;
                                game = GameState();
                                gameOverMenu.isVisible = false;
                                menu.selectedOption = 0;
                            }
                        } else {
                            // Navegar en opciones de game over
                            gameOverMenu.handleInput(event.key.scancode);
                        }
                    } else {
                        // Juego en progreso: manejar movimiento
                        game.handleInput(event.key.scancode);
                    }
                }
            }
        }
        
        // ========== RENDERIZADO SEGÚN ESTADO ==========
        if (gameState == MENU) {
            menu.draw(window);
        } else if (gameState == RULES) {
            rules.draw(window);
        } else if (gameState == PLAYING) {
            // Actualizar lógica del juego
            game.update(0.016f);
            
            // Si el juego terminó, mostrar pantalla de game over
            if (game.gameOver && !gameOverMenu.isVisible) {
                gameOverMenu.show(game.score, game.applesEaten);
            }
            
            // Renderizar juego
            window.clear(sf::Color::Black);
            game.draw(window);
            game.drawUI(window);
            
            // Si hay game over, dibujarlo sobre el juego
            if (gameOverMenu.isVisible) {
                gameOverMenu.draw(window);
            }
        }
        
        window.display();
    }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Excepción: " << e.what() << std::endl;
        return 1;
    }
}