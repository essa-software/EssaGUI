#pragma once

#include <string>

namespace EML {

class Formatter {
public:
    explicit Formatter(std::string input)
        : m_input(std::move(input)) { }

    std::string format() const;

private:
    std::string m_input;
};

}
