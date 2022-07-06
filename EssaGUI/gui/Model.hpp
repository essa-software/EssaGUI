#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <variant>

namespace GUI {

using Variant = std::variant<sf::String, sf::Texture const*>;

class Model {
public:
    virtual ~Model() = default;

    virtual size_t row_count() const = 0;
    virtual size_t column_count() const = 0;
    virtual Variant data(size_t row, size_t column) const = 0;

    struct Column {
        float width = 15;
        sf::String name;
    };

    virtual Column column(size_t column) const = 0;
};

}
