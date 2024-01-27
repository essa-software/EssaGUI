#include "Distance.hpp"

#include <Essa/Geometry/Line2D.hpp>
#include <Essa/Geometry/LineSegment2D.hpp>

namespace Eg {

double distance(Line2D const& line, Util::Point2f const& point) {
    auto [a, b, c] = line.to_axbyc();
    return std::abs(a * point.x() + b * point.y() + c) / std::hypot(a, b);
}

double distance(LineSegment2D const& line, Util::Point2f const& point) {
    auto pproject = line.extension().project(point);
    if (!line.bounding_box().contains(pproject)) {
        return std::min(point.distance(line.point1()), point.distance(line.point2()));
    }
    return distance(point, line.extension());
}

}
