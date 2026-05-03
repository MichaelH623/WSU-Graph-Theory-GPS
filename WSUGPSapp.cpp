#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "Button.h"
#include "Node.h"
#include "Edge.h"
#include "TextBox.h"

namespace fs = std::filesystem;

void recalculateEdges(std::vector<Node>& nodes, std::vector<Edge>& edges, float radius) {
    // Wipe old edges
    edges.clear();
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = i + 1; j < nodes.size(); ++j) {
            // Standard distance formula: sqrt(dx^2 + dy^2)
            float dx = nodes[i].position.x - nodes[j].position.x;
            float dy = nodes[i].position.y - nodes[j].position.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance <= radius) {
                // This call triggers the Edge constructor
                edges.emplace_back(nodes[i].position, nodes[j].position);
            }
        }
    }
}

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
    btnAddNode = std::make_unique<Button>("Add Node", sf::Vector2f(50, 1300), font);
    btnDelNode = std::make_unique<Button>("Delete Node", sf::Vector2f(250, 1300), font);
    btnAddEdge = std::make_unique<Button>("Add Edge", sf::Vector2f(450, 1300), font);
    btnDelEdge = std::make_unique<Button>("Delete Edge", sf::Vector2f(650, 1300), font);
    btnClear = std::make_unique<Button>("Clear Graph", sf::Vector2f(850, 1300), font);
    btnLoad = std::make_unique<Button>("Load Graph", sf::Vector2f(1050, 1300), font);
    btnSave = std::make_unique<Button>("Save Graph", sf::Vector2f(1250, 1300), font);

    // Creates the text box to set edge radius
    auto txtRadius = std::make_unique<TextBox>(sf::Vector2f(450, 1365), font);

    // Stores all the Nodes
    std::vector<Node> nodes;
    // Stores all the Edges
    std::vector<Edge> edges;
    // Determines if they are currently adding a Node
    bool isAddingNode = false;
    // Determines if they are currently adding a Edge
    bool isAddingEdge = false;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Handle Button Clicks
            if (event->is<sf::Event::MouseButtonPressed>()) {
                auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    // Checks if mouse click is inside text box
                    txtRadius->isSelected = txtRadius->box.getGlobalBounds().contains(mousePos);

                    if (btnAddNode->isClicked(mousePos)) {
                        std::cout << "Add Node Clicked\n";
                        isAddingNode = true;
                        std::cout << "User Is Placing Node\n";
                    }
                    else if (isAddingNode) {
                        nodes.emplace_back(mousePos);

                        // Automatically connect based on the radius in the TextBox
                        recalculateEdges(nodes, edges, txtRadius->getValue());

                        isAddingNode = false; // puts the node down on the map
                        std::cout << "Node Added at: " << mousePos.x << ", " << mousePos.y << "\n";
                    }
                    if (btnDelNode->isClicked(mousePos)) {
                        std::cout << "Delete Node Clicked\n";
                    }
                    if (btnAddEdge->isClicked(mousePos)) {
                        std::cout << "Add Edge Clicked\n";
                    }
                    if (btnDelEdge->isClicked(mousePos)) {
                        std::cout << "Delete Edge Clicked\n";
                    }
                    if (btnClear->isClicked(mousePos)) {
                        std::cout << "Clear All Clicked\n";
                    }
                    if (btnLoad->isClicked(mousePos)) {
                        std::cout << "Load Clicked\n";
                    }
                    if (btnSave->isClicked(mousePos)) {
                        std::cout << "Save Clicked\n";
                    }
                }
            }

            // Handle Text Box
            if (event->is<sf::Event::TextEntered>()) {
                auto textEvent = *event->getIf<sf::Event::TextEntered>();
                txtRadius->handleInput(textEvent);
                // recalculateEdges(nodes, edges, txtRadius->getValue());
            }
        }

        window.clear();

        // Draw background
        window.draw(background);
        // Draw Edges and Nodes
        for (auto& edge : edges) {
            edge.draw(window);
        }
        for (auto& node : nodes) {
            node.draw(window);
        }
        // Draw Node that follows mouse
        if (isAddingNode) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            Node ghostNode(mousePos);

            // Set to semi-transparent red (RGBA: 255, 0, 0, 127)
            ghostNode.shape.setFillColor(sf::Color(255, 0, 0, 127));
            ghostNode.draw(window);
        }
        // Draw buttons
        btnAddNode->draw(window);
        btnDelNode->draw(window);
        btnAddEdge->draw(window);
        btnDelEdge->draw(window);
        btnClear->draw(window);
        btnLoad->draw(window);
        btnSave->draw(window);
        // Draw text box
        txtRadius->draw(window);

        window.display();
    }
    return 0;
}