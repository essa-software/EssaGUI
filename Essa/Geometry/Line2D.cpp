#include "Line2D.hpp"

namespace Eg {

Line2D Line2D::from_points(Util::Point2f p1, Util::Point2f p2) {
    assert(p1 != p2);
    return Line2D({ p1.y() - p2.y(), p2.x() - p1.x(), p1.x() * p2.y() - p1.y() * p2.x() });
}

Util::Point2f Line2D::project(Util::Point2f point) const {
    Util::Point2f p0 { is_vertical() ? x_intercept() : 0, is_vertical() ? 0 : solve_for_y(0) };
    auto v = to_vector();
    // https://math.stackexchange.com/a/62718
    auto proj = p0 + v * (point - p0).dot(v) / v.dot(v);
    return proj;
}

}
