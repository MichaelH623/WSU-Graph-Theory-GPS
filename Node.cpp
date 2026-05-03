#include <SFML/Graphics.hpp>

struct Node {
	int id;
	sf::Vector2f position;
	sf::CircleShape shape;
	std::vector<int> neighbors;
};