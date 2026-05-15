#include "CustomBounds.h"

sf::FloatRect getGlobalBounds(const sf::RectangleShape& shape)


{


    return sf::FloatRect(shape.getPosition().x - shape.getOrigin().x * shape.getScale().x, shape.getPosition().y - shape.getOrigin().y * shape.getScale().y, shape.getSize().x * shape.getScale().x, shape.getSize().y * shape.getScale().y);


}
sf::FloatRect getGlobalBounds(const sf::CircleShape& shape)


{


    return sf::FloatRect(shape.getPosition().x - shape.getOrigin().x * shape.getScale().x, shape.getPosition().y - shape.getOrigin().y * shape.getScale().y, shape.getRadius() * 2.f * shape.getScale().x, shape.getRadius() * 2.f * shape.getScale().y);


}
sf::FloatRect getGlobalBounds(const sf::Sprite& sprite)


{
    return sf::FloatRect(
        sprite.getPosition().x - sprite.getOrigin().x * sprite.getScale().x,
        sprite.getPosition().y - sprite.getOrigin().y * sprite.getScale().y,
        static_cast<float>(sprite.getTextureRect().width) * sprite.getScale().x,
        static_cast<float>(sprite.getTextureRect().height) * sprite.getScale().y
    );
}

sf::FloatRect getGlobalBounds(const sf::Text& text)


{
    float w = 310.f;
    if (text.getString() == "Campaign Mode")
    {
        w = 300.f;
    }
    else if (text.getString() == "Quit") w = 80.f;

    float h = 45.f;

    return sf::FloatRect(
        text.getPosition().x - (w / 2.f) * text.getScale().x,
        text.getPosition().y - (h / 2.f) * text.getScale().y,
        w * text.getScale().x,
        h * text.getScale().y
    );
}




