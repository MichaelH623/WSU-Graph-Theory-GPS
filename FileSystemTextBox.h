#pragma once

#include <SFML/Graphics.hpp>
#include <string>

struct FileSystemTextBox {
    sf::RectangleShape box;
    sf::Text displayValue;
    std::string inputString;
    bool isSelected = false;

    FileSystemTextBox(sf::Vector2f pos, sf::Font& font, std::string initialText)
        : displayValue(font, initialText, 18) {

        inputString = initialText; // Set the data

        box.setSize({ 180.f, 40.f });
        box.setFillColor(sf::Color(30, 30, 30));
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color::White);
        box.setPosition(pos);

        displayValue.setFont(font);
        displayValue.setCharacterSize(18);
        displayValue.setString(inputString); // Set the SFML visual text
        displayValue.setFillColor(sf::Color::White);
        displayValue.setPosition({ pos.x + 10.f, pos.y + 5.f });
    }

    void handleInput(sf::Event::TextEntered& textEvent) {
        if (!isSelected) return;

        // Handle Backspace (Unicode 8)
        if (textEvent.unicode == 8) {
            if (!inputString.empty()) inputString.pop_back();
        }
        // Handle standard visible characters (letters, numbers, underscores, etc.)
        else if (textEvent.unicode >= 32 && textEvent.unicode <= 126) {
            // Limit length to keep it inside the box
            if (inputString.length() < 25) {
                inputString += static_cast<char>(textEvent.unicode);
            }
        }

        displayValue.setString(inputString);
    }

    void draw(sf::RenderWindow& window) {
        box.setOutlineColor(isSelected ? sf::Color::Cyan : sf::Color::White);
        window.draw(box);
        window.draw(displayValue);
    }

    // Returns the actual text for filenames
    std::string getStringValue() {
        return inputString;
    }
};