#pragma once

#include <SFML/Graphics.hpp>
#include <string>

struct Button {
    sf::RectangleShape shape;
    sf::Text label;
    std::string name;

    Button(std::string text, sf::Vector2f pos, sf::Font& font)
        : label(font, text, 18)
    {
        name = text;

        // Configure the box
        shape.setSize({ 180.f, 60.f });
        shape.setFillColor(sf::Color(70, 70, 70)); // Dark grey
        shape.setOutlineThickness(2.f);
        shape.setOutlineColor(sf::Color::White);
        shape.setPosition(pos);

        // Configure the text appearance
        label.setFillColor(sf::Color::White);

        // Center text in button using SFML 3 .position and .size
        sf::FloatRect textRect = label.getLocalBounds();
        label.setOrigin({ textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f });
        label.setPosition({ pos.x + 90.f, pos.y + 30.f });
    }

    bool isClicked(sf::Vector2f mousePos) {
        return shape.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(label);
    }
};