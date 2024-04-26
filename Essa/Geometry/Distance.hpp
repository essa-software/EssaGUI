#pragma once

#include <EssaUtil/CoordinateSystem.hpp>

namespace Eg {

class Line2D;
class LineSegment2D;

double distance(Line2D const&, Util::Point2f const&);
inline double distance(Util::Point2f const& a, Line2D const& b) {
    return distance(b, a);
}
double distance(LineSegment2D const&, Util::Point2f const&);
inline double distance(Util::Point2f const& a, LineSegment2D const& b) {
    return distance(b, a);
}

}
