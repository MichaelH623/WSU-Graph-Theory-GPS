#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

int main() {
    sf::RenderWindow window(sf::VideoMode({ 2000, 1500 }), "WSU Graph Project");

    sf::Texture campusTexture;
    fs::path mapPath = fs::current_path() / "Assets" / "WSU-Campus-Map-2024.png";

    //Open the file using Standard C++ 
    std::ifstream file(mapPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "File Failed To Pull" << std::endl;
        return -1;
    }
    //Read the file into a memory buffer
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cerr << "Failed to read file data into buffer." << std::endl;
        return -1;
    }
    //Tell SFML to load from that memory buffer
    if (!campusTexture.loadFromMemory(buffer.data(), buffer.size())) {
        std::cerr << "SFML failed to parse the image data from memory" << std::endl;
        return -1;
    }
    sf::Sprite background(campusTexture);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(background);
        window.display();
    }
    return 0;
}