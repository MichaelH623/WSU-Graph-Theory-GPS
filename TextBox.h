#pragma once

#include <SFML/Graphics.hpp>

struct TextBox {
    sf::RectangleShape box;
    sf::Text displayValue;
    std::string inputString = "100"; // Default radius
    bool isSelected = false;

    TextBox(sf::Vector2f pos, sf::Font& font)
        : displayValue(font, "100", 18) {
        box.setSize({ 180.f, 40.f });
        box.setFillColor(sf::Color::Black);
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color::White);
        box.setPosition(pos);

        // Position the text relative to the box
        displayValue.setFillColor(sf::Color::White);
        displayValue.setPosition({ pos.x + 10.f, pos.y + 5.f });
    }

    void handleInput(sf::Event::TextEntered& textEvent) {
        if (!isSelected) return;

        // Handle Backspace
        if (textEvent.unicode == 8) {
            if (!inputString.empty()) inputString.pop_back();
        }
        // Handle Numbers only
        else if (textEvent.unicode >= 48 && textEvent.unicode <= 57) {
            if (inputString.length() < 5) { // Limit length
                inputString += static_cast<char>(textEvent.unicode);
            }
        }
        displayValue.setString(inputString);
    }

    void draw(sf::RenderWindow& window) {
        box.setOutlineColor(isSelected ? sf::Color::Yellow : sf::Color::White);
        window.draw(box);
        window.draw(displayValue);
    }

    float getValue() {
        return inputString.empty() ? 0.f : std::stof(inputString);
    }
};