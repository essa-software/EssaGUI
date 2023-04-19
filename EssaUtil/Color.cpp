#include "Color.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace Util {

Util::UString Color::to_html_string() const {
    std::ostringstream oss;
    oss << "#" << std::hex << std::setfill('0');
    oss << std::setw(2) << (int)r << std::setw(2) << (int)g << std::setw(2) << (int)b;
    return Util::UString { oss.str() };
}

Color Color::rgb_blend(Color const& l, Color const& r, float how_much_r) {
    how_much_r = std::clamp(how_much_r, 0.f, 1.f);
    return l * (1 - how_much_r) + r * how_much_r;
}

}
