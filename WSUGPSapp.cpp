#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "Button.cpp"
#include "Node.cpp"
#include "Edge.cpp"

namespace fs = std::filesystem;

int main() {
    sf::RenderWindow window(sf::VideoMode({ 2000, 1500 }), "WSU Graph Project");

    // Load the Font arial
    sf::Font font;
    fs::path fontPath = fs::current_path() / "Assets" / "arial.ttf"; // Or "arial.ttf"

    // Open the file with Standard C++
    std::ifstream fontFile(fontPath, std::ios::binary | std::ios::ate);
    if (!fontFile.is_open()) {
        std::cerr << "CRITICAL: Could not find font at " << fs::absolute(fontPath) << std::endl;
        return -1;
    }

    // Read into buffer
    std::streamsize fSize = fontFile.tellg();
    fontFile.seekg(0, std::ios::beg);
    std::vector<char> fontBuffer(fSize);
    fontFile.read(fontBuffer.data(), fSize);

    // Load from memory
    if (!font.openFromMemory(fontBuffer.data(), fontBuffer.size())) {
        std::cerr << "SFML failed to parse font data from memory!" << std::endl;
        return -1;
    }

    // Load the Campus Map
    sf::Texture campusTexture;
    fs::path mapPath = fs::current_path() / "Assets" / "WSU-Campus-Map-2024.png";
    std::ifstream file(mapPath, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        file.read(buffer.data(), size);
        campusTexture.loadFromMemory(buffer.data(), buffer.size());
    }
    sf::Sprite background(campusTexture);
    
    // Declaring the 7 buttons
    std::unique_ptr<Button> btnAddNode;
    std::unique_ptr<Button> btnDelNode;
    std::unique_ptr<Button> btnAddEdge;
    std::unique_ptr<Button> btnDelEdge;
    std::unique_ptr<Button> btnClear;
    std::unique_ptr<Button> btnLoad;
    std::unique_ptr<Button> btnSave;

    // Initialize them one by one
    btnAddNode = std::make_unique<Button>("Add Node", sf::Vector2f(50, 1400), font);
    btnDelNode = std::make_unique<Button>("Delete Node", sf::Vector2f(250, 1400), font);
    btnAddEdge = std::make_unique<Button>("Add Edge", sf::Vector2f(450, 1400), font);
    btnDelEdge = std::make_unique<Button>("Delete Edge", sf::Vector2f(650, 1400), font);
    btnClear = std::make_unique<Button>("Clear Graph", sf::Vector2f(850, 1400), font);
    btnLoad = std::make_unique<Button>("Load Graph", sf::Vector2f(1050, 1400), font);
    btnSave = std::make_unique<Button>("Save Graph", sf::Vector2f(1250, 1400), font);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Handle Button Clicks
            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    if (btnAddNode->isClicked(mousePos)) std::cout << "Add Node Clicked\n";
                    if (btnDelNode->isClicked(mousePos)) std::cout << "Delete Node Clicked\n";
                    if (btnAddEdge->isClicked(mousePos)) std::cout << "Add Edge Clicked\n";
                    if (btnDelEdge->isClicked(mousePos)) std::cout << "Delete Edge Clicked\n";
                    if (btnClear->isClicked(mousePos)) std::cout << "Clear All Clicked\n";
                    if (btnLoad->isClicked(mousePos)) std::cout << "Load Clicked\n";
                    if (btnSave->isClicked(mousePos)) std::cout << "Save Clicked\n";
                }
            }
        }

        window.clear();

        // Draw background
        window.draw(background);
        // Draw buttons
        btnAddNode->draw(window);
        btnDelNode->draw(window);
        btnAddEdge->draw(window);
        btnDelEdge->draw(window);
        btnClear->draw(window);
        btnLoad->draw(window);
        btnSave->draw(window);

        window.display();
    }
    return 0;
}