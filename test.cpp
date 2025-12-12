#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::Texture tex;
    
    std::cout << "Intentando cargar: C:/Users/Bienvenido/Desktop/SNAKEvsBLOCE/assets/images/Menu.Fondo.png.png" << std::endl;
    
    if (!tex.loadFromFile("C:/Users/Bienvenido/Desktop/SNAKEvsBLOCE/assets/images/Menu.Fondo.png.png")) {
        std::cout << "ERROR: No se pudo cargar la textura" << std::endl;
        return 1;
    }
    
    std::cout << "Textura cargada exitosamente" << std::endl;
    
    sf::RenderWindow window(sf::VideoMode(800, 600), "Test");
    sf::Sprite sprite(tex);
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear();
        window.draw(sprite);
        window.display();
    }
    
    return 0;
}
