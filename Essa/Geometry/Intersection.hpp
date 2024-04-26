#pragma once

#include <Essa/Geometry/Line2D.hpp>
#include <Essa/Geometry/LineSegment2D.hpp>
#include <variant>

namespace Eg {

class Line2DIntersection {
public:
    enum SpecialValue { Overlapping, Distinct };

    Line2DIntersection(Util::Point2f p)
        : m_v(p) { }

    Line2DIntersection(SpecialValue p)
        : m_v(p) { }

    bool is_point() const {
        return std::holds_alternative<Util::Point2f>(m_v);
    }
    bool operator==(Line2DIntersection const& l2l2i) const = default;
    bool operator==(Util::Point2f const& point) const {
        return is_point() && std::get<Util::Point2f>(m_v) == point;
    }
    operator Util::Point2f() const {
        return std::get<Util::Point2f>(m_v);
    }
    Util::Point2f point() const {
        return *this;
    }

private:
    std::variant<Util::Point2f, SpecialValue> m_v;
};

Line2DIntersection intersection(Line2D const&, Line2D const&);
Line2DIntersection intersection(LineSegment2D const&, Line2D const&);
inline Line2DIntersection intersection(Line2D const& a, LineSegment2D const& b) {
    return intersection(b, a);
}
Line2DIntersection intersection(LineSegment2D const&, LineSegment2D const&);

}
