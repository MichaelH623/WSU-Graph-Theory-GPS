#pragma once

#include <SFML/Graphics.hpp>

struct Edge {
    sf::VertexArray line;

    // Initialize VertexArray with 2 points
    Edge(sf::Vector2f start, sf::Vector2f end)
        : line(sf::PrimitiveType::Lines, 2)
    {
        line[0].position = start;
        line[0].color = sf::Color::Black;
        line[1].position = end;
        line[1].color = sf::Color::Black;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(line);
    }
};