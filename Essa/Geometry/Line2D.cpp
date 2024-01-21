#include "Line2D.hpp"

namespace Eg {

Line2D Line2D::from_points(Util::Point2f p1, Util::Point2f p2) {
    assert(p1 != p2);
    return Line2D({ p1.y() - p2.y(), p2.x() - p1.x(), p1.x() * p2.y() - p1.y() * p2.x() });
}

}
