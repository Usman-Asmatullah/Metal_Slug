#pragma once
#include <SFML/Graphics.hpp>

sf::FloatRect getGlobalBounds(const sf::RectangleShape& shape);
sf::FloatRect getGlobalBounds(const sf::CircleShape& shape);
sf::FloatRect getGlobalBounds(const sf::Sprite& sprite);
sf::FloatRect getGlobalBounds(const sf::Text& text);



