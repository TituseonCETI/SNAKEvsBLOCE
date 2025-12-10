#include <SFML/Graphics.hpp>

int main() {
    // Crear una ventana
    sf::RenderWindow window(sf::VideoMode(800, 600), "Prueba SFML 3.0.2");

    // Limitar FPS (opcional)
    window.setFramerateLimit(60);

    // Loop principal
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Cerrar al presionar X
            if (event.type == sf::Event::Closed)
                window.close();

            // Cerrar al presionar ESC
            if (event.type == sf::Event::KeyPressed && event.key.scancode == sf::Keyboard::Scan::Escape)
                window.close();
        }

        // Pintar fondo negro
        window.clear(sf::Color::Black);

        // Mostrar lo dibujado
        window.display();
    }
    return 0;
}