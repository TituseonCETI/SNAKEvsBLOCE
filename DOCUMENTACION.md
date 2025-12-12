# 📚 DOCUMENTACIÓN - SNAKE vs BLOCKS

## 🎮 Descripción General
**Snake vs Blocks** es un juego mejorado de la serpiente clásica, desarrollado en C++ usando SFML 3.0.2. El juego incluye obstáculos, power-ups especiales, y un sistema de puntuación progresivo.

---

## 📁 Estructura del Proyecto

```
SNAKEvsBLOCE/
├── src/
│   └── main.cpp              # Código principal del juego
├── bin/
│   └── main.exe              # Ejecutable compilado
├── assets/
│   └── images/               # Imágenes y texturas
├── makefile                  # Archivo de compilación
└── README.md
```

---

## 🔧 Compilación y Ejecución

### Compilar:
```bash
make runmain
```

### Ejecutar directamente:
```bash
./bin/main.exe
```

---

## 🎮 Controles del Juego

| Tecla | Acción |
|-------|--------|
| **↑ Arriba** | Mover serpiente hacia arriba |
| **↓ Abajo** | Mover serpiente hacia abajo |
| **← Izquierda** | Mover serpiente hacia izquierda |
| **→ Derecha** | Mover serpiente hacia derecha |
| **ENTER** | Seleccionar opción del menú |
| **↑↓** (Menú) | Navegar opciones |
| **ESC** | Volver al menú / Salir |
| **R** (Game Over) | Reiniciar juego |

---

## 🏗️ Estructura del Código

### 1️⃣ SECCIÓN: Librerías e Inclusiones (líneas 1-8)

Incluye todas las librerías necesarias:
- **SFML/Graphics.hpp**: Gráficos y renderizado
- **vector**: Contenedores dinámicos
- **cstdlib, ctime**: Generación de números aleatorios
- **string, sstream**: Manejo de texto

```cpp
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
```

### 2️⃣ SECCIÓN: Constantes de Configuración (líneas 10-20)

Definen valores fijos del juego que NO cambian:

```cpp
const int GRID_SIZE = 20;           // Tamaño de cada celda (20x20 píxeles)
const int BASE_WINDOW_WIDTH = 800;  // Ancho base sin escala
const int BASE_WINDOW_HEIGHT = 600; // Alto base sin escala
const int PANEL_WIDTH = 280;        // Ancho del panel lateral derecho
```

### 3️⃣ SECCIÓN: Variables Globales (líneas 22-47)

Variables que se pueden modificar según la resolución:

```cpp
int WINDOW_WIDTH = 800;     // Ancho del área de juego
int WINDOW_HEIGHT = 600;    // Alto del área de juego
int SCREEN_WIDTH = 1080;    // Ancho total (juego + panel)
int SCREEN_HEIGHT = 720;    // Alto total

float SCALE_X = 1.0f;       // Factor de escala horizontal
float SCALE_Y = 1.0f;       // Factor de escala vertical

sf::Texture tex;            // Textura global para fondos
```

### 4️⃣ SECCIÓN: Enumeraciones (líneas 49-69)

Define tipos que facilitan el código:

#### Estados del Juego:
```cpp
enum GameState_Type {
    MENU,       // Pantalla de menú
    PLAYING,    // Jugando
    RULES,      // Mostrando reglas
    GAME_OVER   // Pantalla de fin de juego
};
```

#### Tipos de Power-ups:
```cpp
enum PowerUpType {
    WALL_PASS,           // Atravesar paredes (10s)
    DOUBLE_SCORE,        // Puntos dobles (10s)
    MAGNET,              // Atrae manzanas (10s)
    OBSTACLE_DESTROYER   // Destruye obstáculos (instantáneo)
};
```

### 5️⃣ SECCIÓN: Estructuras de Datos (líneas 71-126)

#### **SnakeSegment** - Un segmento de la serpiente
```cpp
struct SnakeSegment {
    int x, y;  // Posición en el grid
    
    // Operador == para comparar posiciones
    bool operator==(const SnakeSegment& other) const {
        return x == other.x && y == other.y;
    }
};
```

#### **Block** - Una manzana para comer
```cpp
struct Block {
    int x, y;           // Posición en píxeles
    int width, height;  // Dimensiones (típicamente GRID_SIZE)
};
```

#### **PowerUp** - Un poder especial
```cpp
struct PowerUp {
    int x, y;           // Posición en píxeles
    PowerUpType type;   // Qué tipo de poder es
    int width, height;  // Dimensiones
};
```

#### **Obstacle** - Un obstáculo que mata
```cpp
struct Obstacle {
    int x, y;           // Posición en píxeles
    int width, height;  // Dimensiones
};
```

### 6️⃣ FUNCIÓN: calculateScaling() (líneas 128-135)

Calcula los factores de escala para adaptar el juego a diferentes resoluciones:

```cpp
void calculateScaling() {
    SCREEN_WIDTH = WINDOW_WIDTH + PANEL_WIDTH;
    SCALE_X = (float)WINDOW_WIDTH / BASE_WINDOW_WIDTH;
    SCALE_Y = (float)WINDOW_HEIGHT / BASE_WINDOW_HEIGHT;
}
```

**¿Qué hace?**
- Suma el ancho del panel al ancho del juego
- Calcula factores de escala (ej: si la pantalla es el doble, SCALE = 2)

---

## 🎯 CLASES PRINCIPALES

### 📋 **Clase: Menu**

Gestiona la pantalla de menú principal.

**Variables:**
```cpp
int selectedOption = 0;  // 0=Iniciar, 1=Reglas, 2=Salir
```

**Funciones:**
```cpp
void handleInput(sf::Keyboard::Scancode key)  // Procesa flecha arriba/abajo
void draw(sf::RenderWindow& window)           // Dibuja el menú
int getSelectedOption()                       // Retorna opción seleccionada
```

**¿Cómo funciona?**
1. Lee entrada del usuario (↑ ↓)
2. Cambia `selectedOption` (0, 1 o 2)
3. Dibuja 3 rectángulos coloreados (verde, amarillo, rojo)
4. Resalta el seleccionado con borde blanco

---

### 📋 **Clase: Rules**

Muestra la pantalla de reglas.

**Funciones:**
```cpp
void draw(sf::RenderWindow& window)  // Dibuja la pantalla de reglas
```

**¿Qué dibuja?**
- Fondo oscuro
- Título gris
- Caja con espacio para texto
- Caja de instrucciones en la parte inferior

---

### 🎮 **Clase: GameState** (LA MÁS IMPORTANTE)

Contiene TODA la lógica del juego.

#### **Variables de Estado:**

**Datos de la Serpiente:**
```cpp
std::vector<SnakeSegment> snake;  // Todos los segmentos
int direction = 1;                // 0=Arriba, 1=Derecha, 2=Abajo, 3=Izquierda
int nextDirection = 1;            // Siguiente dirección
```

**Datos del Mapa:**
```cpp
std::vector<Block> blocks;        // Manzanas
std::vector<PowerUp> powerUps;    // Poderes
std::vector<Obstacle> obstacles;  // Obstáculos
```

**Puntuación y Progreso:**
```cpp
int score = 0;          // Puntos totales
int applesEaten = 0;    // Manzanas comidas
bool gameOver = false;  // ¿Ha terminado?
int speedLevel = 1;     // Velocidad actual (1, 2, 3...)
```

**Timers de Spawn (Para generar elementos):**
```cpp
float blockSpawnTimer = 0;              // Timer de manzanas
float blockSpawnDelay = 1.0f;           // Cada 1 segundo aparece una

float powerUpSpawnTimer = 0;            // Timer de power-ups
float powerUpSpawnDelay = 15.0f;        // Cada 15 segundos

float obstacleSpawnTimer = 0;           // Timer de obstáculos
float obstacleSpawnDelay = 8.0f;        // Cada 8 segundos

float obstacleDestroyerSpawnTimer = 0;  // Timer especial
float obstacleDestroyerSpawnDelay = 30.0f;  // Cada 30s si hay 15+ obstáculos
```

**Power-ups Activos (Duran 10 segundos):**
```cpp
bool wallPassActive = false;       // Puede atravesar paredes
bool doubleScoreActive = false;    // Puntos dobles
bool magnetActive = false;         // Atrae manzanas
```

#### **Funciones Principales:**

```cpp
GameState()                                // Constructor: inicia serpiente en centro
void handleInput(sf::Keyboard::Scancode)   // Procesa controles (↑ ↓ ← →)
void update(float deltaTime)               // ACTUALIZA TODA LA LÓGICA CADA FRAME
void draw(sf::RenderWindow&)               // DIBUJA elementos del juego
void drawUI(sf::RenderWindow&)             // DIBUJA panel lateral con info
```

#### **¿QUÉ HACE update() ?**

Es la función más importante. Se ejecuta cada frame (60 veces por segundo).

**Pasos que realiza:**

1. **Actualizar Power-ups Activos**
   - Incrementa timers de Wall Pass, Double Score, Magnet
   - Si llegan a 10 segundos, desactívalos

2. **Lógica del Magnet**
   - Si Magnet está activo, acerca manzanas a la cabeza

3. **Controlar Velocidad de Movimiento**
   - Aumenta velocidad cada 10 manzanas comidas
   - Usa `moveCounter` y `moveDelay` para ralentizar

4. **Spawn de Elementos** (cada 1-30 segundos):
   - **Manzanas**: Cada 1 segundo en posición aleatoria
   - **Power-ups**: Cada 15 segundos (tipo aleatorio)
   - **Obstáculos**: Cada 8 segundos (máximo 20)
   - **Obstacle Destroyer**: Cada 30 segundos si hay 15+ obstáculos

5. **Mover la Serpiente**
   - Crea nueva cabeza según dirección
   - Verifica colisiones (paredes, sí mismo, obstáculos)
   - Si Wall Pass activo, teleporta al otro lado
   - Inserta cabeza al inicio, quita cola (a menos que comió)

6. **Comer Manzanas**
   - Detecta colisión cabeza-manzana
   - Suma puntos (10 o 20 si Double Score)
   - Crece la serpiente (no quita cola)

7. **Recoger Power-ups**
   - Detecta colisión cabeza-power-up
   - Activa el efecto del poder
   - Si es OBSTACLE_DESTROYER: `obstacles.clear()`

---

### 📋 **Clase: GameOverMenu**

Muestra la pantalla de fin de juego.

**Variables:**
```cpp
bool isVisible = false;      // ¿Se muestra?
int finalScore = 0;          // Puntuación final
int finalApplesEaten = 0;    // Manzanas comidas
int selectedOption = 0;      // 0=Reiniciar, 1=Salir
```

**Funciones:**
```cpp
void show(int score, int applesEaten)   // Activa el menú con datos
void handleInput(sf::Keyboard::Scancode) // Navega opciones
void draw(sf::RenderWindow&)             // Dibuja el menú
```

---

## 🎨 SISTEMA DE COLORES

| Color | Significado |
|-------|------------|
| **Verde** | Serpiente |
| **Rojo** | Manzanas |
| **Amarillo** | Power-up WALL_PASS |
| **Magenta** | Power-up DOUBLE_SCORE |
| **Naranja** | Power-up MAGNET |
| **Blanco** | Power-up OBSTACLE_DESTROYER |
| **Cyan** | Obstáculos |

---

## ⚙️ SISTEMA DE VELOCIDAD

La velocidad aumenta automáticamente:

```
Manzanas comidas → Velocidad
0-9              → Nivel 1 (moveDelay=10)
10-19            → Nivel 2 (moveDelay=9.5)
20-29            → Nivel 3 (moveDelay=9.0)
...
```

**moveDelay** es cuántos frames debe esperar antes de mover.

---

## 🎁 SISTEMA DE POWER-UPS

### 1. **WALL_PASS** (Amarillo)
- **Duración**: 10 segundos
- **Efecto**: La serpiente puede atravesar paredes y teleportarse

### 2. **DOUBLE_SCORE** (Magenta)
- **Duración**: 10 segundos
- **Efecto**: Cada manzana vale 20 puntos en lugar de 10

### 3. **MAGNET** (Naranja)
- **Duración**: 10 segundos
- **Efecto**: Todas las manzanas se atraen hacia la cabeza automáticamente

### 4. **OBSTACLE_DESTROYER** (Blanco)
- **Duración**: Instantáneo (no dura)
- **Efecto**: Elimina TODOS los obstáculos del mapa
- **Bonus**: +50 puntos
- **Aparece**: Cada 30 segundos si hay 15+ obstáculos

---

## 🔄 FLUJO DEL JUEGO PRINCIPAL

```
INICIO
  ↓
CARGAR TEXTURAS
  ↓
BUCLE PRINCIPAL (while window.isOpen())
  ├─ PROCESAR EVENTOS (teclas, cierre)
  │
  ├─ IF gameState == MENU
  │   └─ Dibujar menú
  │       └─ Si ENTER: cambiar estado
  │
  ├─ IF gameState == RULES
  │   └─ Dibujar reglas
  │       └─ Si ENTER: volver a MENU
  │
  ├─ IF gameState == PLAYING
  │   ├─ game.update(0.016f)      ← ACTUALIZA TODO
  │   ├─ game.draw(window)        ← DIBUJA JUEGO
  │   └─ game.drawUI(window)      ← DIBUJA PANEL
  │
  ├─ window.display()             ← MOSTRAR EN PANTALLA
  │
  └─ (Volver al inicio del bucle)
  
FIN (Al cerrar ventana)
```

---

## 📊 ESTADÍSTICAS Y PANEL LATERAL

El panel derecho (280 píxeles de ancho) muestra:

1. **Barra de Puntuación** (Verde)
   - Crece según los puntos

2. **Contador de Manzanas** (Rojo)
   - Mostrando cubos pequeños (máximo 50)

3. **Puntos por Manzana** (Verde)
   - Barras que muestran 10 o 20 puntos

4. **Indicador de Velocidad** (Amarillo)
   - Barras que aumentan con la velocidad

5. **Timer de Power-ups Activos**
   - Barra que disminuye cada segundo

---

## 🐛 DEBUGGING Y LOGS

El código usa `std::cout` para algunos mensajes (requiere compilación sin modo GUI):

```cpp
std::cout << "Cargando texturas..." << std::endl;
```

Los mensajes se muestran en la consola durante la ejecución.

---

## 🎯 TIPS PARA JUGAR

✅ **Objetivos:**
- Come tantas manzanas como puedas
- Evita obstáculos
- Recolecta power-ups para ventajas
- Aumenta tu velocidad y puntuación

⚠️ **Cuidados:**
- No choque con la serpiente a sí misma
- Los obstáculos causan game over
- Power-ups duran solo 10 segundos (excepto OBSTACLE_DESTROYER)

💡 **Estrategia:**
- Cuando hay muchos obstáculos, espera el OBSTACLE_DESTROYER (blanco)
- El MAGNET es útil para alcanzar manzanas lejanas
- DOUBLE_SCORE multiplica los puntos, ¡aprovéchalo!

---

## 📝 INFORMACIÓN TÉCNICA

- **Lenguaje**: C++11 o superior
- **Librería Gráfica**: SFML 3.0.2
- **Compilador**: g++ (MinGW en Windows)
- **Resolución**: 1080x720 píxeles fijos
- **FPS**: 60 frames por segundo
- **Delta Time**: 0.016 segundos (1/60)

---

## 🔗 Referencias Útiles

- [SFML Documentation](https://www.sfml-dev.org/documentation/3.0/)
- [C++ Reference](https://en.cppreference.com/)

---

**¡Disfruta el juego!** 🎮✨
