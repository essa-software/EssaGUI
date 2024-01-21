#include "Distance.hpp"

#include <Essa/Geometry/Line2D.hpp>

namespace Eg {

double distance(Line2D const& line, Util::Point2f const& point) {
    auto [a, b, c] = line.to_axbyc();
    return std::abs(a * point.x() + b * point.y() + c) / std::hypot(a, b);
}

}
