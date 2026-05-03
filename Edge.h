#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

struct Edge {
    sf::Vertex line[2];
    float weight;
    // The IDs of the connected nodes
    int u, v;

    Edge(sf::Vector2f p1, sf::Vector2f p2, int id1, int id2) : u(id1), v(id2) {
        // Set positions directly
        line[0].position = p1;
        line[1].position = p2;

        // Set colors directly
        line[0].color = sf::Color::Black;
        line[1].color = sf::Color::Black;

        // Calculate Euclidean distance which will be the default weight value
        float dx = p1.x - p2.x;
        float dy = p1.y - p2.y;
        weight = std::sqrt(dx * dx + dy * dy);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
};