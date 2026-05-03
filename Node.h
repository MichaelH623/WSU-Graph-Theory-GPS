#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

struct Node {
    int id;
    sf::Vector2f position;
    sf::CircleShape shape;
    bool isSelected;
    std::string name;

    // Node constructor
    Node(sf::Vector2f pos, int nodeId) {
        id = nodeId;
        position = pos;
        isSelected = false;
        shape.setRadius(10.f);
        shape.setFillColor(sf::Color::Red);
        shape.setOrigin({ 10.f, 10.f });
        shape.setPosition(pos);
        name = std::to_string(nodeId);
    }

    void draw(sf::RenderWindow& window) {
        // Change color if selected to "light it up"
        shape.setFillColor(isSelected ? sf::Color::Yellow : sf::Color::Red);
        window.draw(shape);
    }
};