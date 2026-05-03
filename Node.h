#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

struct Node {
    int id;
    sf::Vector2f position;
    sf::CircleShape shape;
    std::vector<int> neighbors;

    // Node constructor
    Node(sf::Vector2f pos) {
        position = pos;

        // Visual setup
        shape.setRadius(10.f);
        shape.setFillColor(sf::Color::Red);

        // Center the circle so it sticks to the mouse by its middle
        shape.setOrigin({ 10.f, 10.f });
        shape.setPosition(pos);
    }

    // Nodes draw method
    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};