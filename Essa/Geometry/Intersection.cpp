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

}
