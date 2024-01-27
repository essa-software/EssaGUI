#include "Intersection.hpp"
#include <limits>

namespace Eg {

Line2DIntersection intersection(Line2D const& line1, Line2D const& line2) {
    auto abc1 = line1.to_axbyc();
    auto abc2 = line2.to_axbyc();
    auto det = abc1.a * abc2.b - abc1.b * abc2.a;
    auto detx = -abc1.c * abc2.b + abc1.b * abc2.c;
    auto dety = -abc1.a * abc2.c + abc1.c * abc2.a;
    if (std::abs(det) < std::numeric_limits<float>::epsilon()) {
        if (std::abs(detx) < std::numeric_limits<float>::epsilon() && std::abs(dety) < std::numeric_limits<float>::epsilon()) {
            return Line2DIntersection::Overlapping;
        }
        return Line2DIntersection::Distinct;
    }
    return { { detx / det, dety / det } };
}

Line2DIntersection intersection(LineSegment2D const& line1, Line2D const& line2) {
    auto intersection = Eg::intersection(line2, line1.extension());
    if (intersection.is_point()) {
        if (line1.bounding_box().contains(intersection.point())) {
            return intersection.point();
        }
        return Line2DIntersection::Distinct;
    }
    return intersection;
}

Line2DIntersection intersection(LineSegment2D const& line1, LineSegment2D const& line2) {
    auto intersection = Eg::intersection(line1.extension(), line2.extension());
    if (intersection.is_point()) {
        if (line1.bounding_box().contains(intersection.point()) && line2.bounding_box().contains(intersection.point())) {
            return intersection.point();
        }
        return Line2DIntersection::Distinct;
    }
    if (intersection == Line2DIntersection::Overlapping) {
        if (line1.bounding_box().contains(line2.point1()) || line1.bounding_box().contains(line2.point2())) {
            return Line2DIntersection::Overlapping;
        }
        return Line2DIntersection::Distinct;
    }
    return intersection;
}

}
