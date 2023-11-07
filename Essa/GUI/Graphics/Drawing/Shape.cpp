#include "Shape.hpp"

namespace Gfx::Drawing {

RoundingResult round(RoundingSettings settings) {
    // fmt::print("--- ROUND ---\n");
    auto& A = settings.left;
    auto& B = settings.right;
    auto& C = settings.tip;
    assert(A != B);
    assert(B != C);
    assert(A != C);
    auto& r = settings.radius;
    // fmt::print("A={} B={} C={} r={}\n", fmt::streamed(A), fmt::streamed(B), fmt::streamed(C), r);

    // 1)
    auto d_a = A - C;
    auto d_b = B - C;
    // fmt::print("DA={}, DB={}\n", fmt::streamed(d_a), fmt::streamed(d_b));

    auto tg_gamma_rec = -(d_a.x() * d_b.x() + d_a.y() * d_b.y()) / (d_a.y() * d_b.x() - d_a.x() * d_b.y());

    // 2a)
    auto cos_gamma = static_cast<float>(d_a.normalized().dot(d_b.normalized()));
    auto sin_gamma = static_cast<float>(tg_gamma_rec == 0 ? 1 : cos_gamma / tg_gamma_rec);
    // fmt::print("sin={}, cos={}\n", sin_gamma, cos_gamma);

    // 2b)
    auto tg_half_gamma = 1 / sin_gamma - tg_gamma_rec;

    // 2c)
    // fmt::print("1/tg y = {}; tg 1/2y = {}\n", tg_gamma_rec, tg_half_gamma);
    // fmt::print("r = {}\n", r);
    auto expected_a = std::abs(r / tg_half_gamma);
    auto a = expected_a;

    // Scale everything down if doesn't fit.
    a = std::min<float>(a, static_cast<float>(d_a.length()) / 2);
    a = std::min<float>(a, static_cast<float>(d_b.length()) / 2);
    r *= a / expected_a;
    // fmt::print("a ..= {} {}\n", expected_a, a);

    // 3)
    auto ca_p = d_a.with_length(a);
    auto cb_p = d_b.with_length(a);
    // fmt::print("ca = {} {}\n", fmt::streamed(ca_p), fmt::streamed(cb_p));

    // 4a)
    auto d = std::hypot(r, a);

    // 4b)
    auto center = C + (ca_p + cb_p).with_length(d);

    // 5)
    auto alpha = ((C + ca_p) - center).angle();
    auto beta = ((C + cb_p) - center).angle();
    if (beta - alpha > 180_deg) {
        alpha += 360_deg;
    }
    if (alpha - beta > 180_deg) {
        beta += 360_deg;
    }
    assert(std::abs((alpha - beta).rad()) < M_PI + 10e-6);

    // fmt::print("{} {}\n", fmt::streamed(settings.tip), fmt::streamed(center));
    return { center, static_cast<float>(alpha.rad()), static_cast<float>(beta.rad()), r };
}

namespace {

std::vector<Util::Point2f> calculate_vertices_for_rounded_shape(Drawing::Shape const& shape) {
    auto round_radius_for_vertex = [&](size_t idx) {
        if (shape.point_count() == 4) {
            switch (idx) {
            case 0:
                return shape.outline().round_radius().top_left;
            case 1:
                return shape.outline().round_radius().top_right;
            case 2:
                return shape.outline().round_radius().bottom_right;
            case 3:
                return shape.outline().round_radius().bottom_left;
            default:
                ESSA_UNREACHABLE;
            }
        }

        // We don't support separate round radiuses for other shapes
        // than rectangle, for now.
        return shape.outline().round_radius().top_left;
    };

    // Don't allow duplicates, otherwise rounding algorithms falls apart
    auto input_vertices = shape.points().to_vector();
    {
        auto const [first, last] = std::ranges::unique(input_vertices, [](auto l, auto r) { return l.is_approximately_equal(r); });
        input_vertices.erase(first, last);
    }
    while (!input_vertices.empty() && input_vertices.front() == input_vertices.back()) {
        input_vertices.pop_back();
    }
    if (input_vertices.size() < 3) {
        return {};
    }

    std::vector<Util::Point2f> output_vertices;
    for (size_t s = 0; s < input_vertices.size(); s++) {
        auto r = round_radius_for_vertex(s);

        auto tip = input_vertices[s];
        if (r == 0) {
            output_vertices.push_back(tip);
            continue;
        }

        auto left = input_vertices[s == 0 ? input_vertices.size() - 1 : s - 1];
        auto right = input_vertices[s == input_vertices.size() - 1 ? 0 : s + 1];

        auto rounding = round({ left, right, tip, r });

        constexpr size_t RoundingResolution = 12;
        for (size_t s = 0; s <= RoundingResolution; s++) {
            float angle = rounding.angle_start
                + (rounding.angle_end - rounding.angle_start) * static_cast<float>(s) / static_cast<float>(RoundingResolution);
            // fmt::print("{}\n", fmt::streamed(rounding.center));
            auto point = rounding.center + Util::Vector2f::create_polar(Util::Angle::radians(angle), rounding.scaled_radius);
            output_vertices.push_back(point);
        }
    }
    {
        auto [first, last] = std::ranges::unique(output_vertices, [](auto l, auto r) { return l.is_approximately_equal(r); });
        input_vertices.erase(first, last);
    }
    while (!output_vertices.empty() && output_vertices.front() == output_vertices.back()) {
        output_vertices.pop_back();
    }
    return output_vertices;
}

}
Shape& Shape::set_fill(Fill fill) {
    // FIXME: Color shouldn't invalidate
    if (m_fill.texture() != fill.texture() || m_fill.texture_rect() != fill.texture_rect() || m_fill.color() != fill.color()
        || (!m_fill.is_visible() && fill.is_visible())) {
        invalidate();
    }
    m_fill = fill;
    return *this;
}

void Shape::generate_fill_vertices() const {
    auto fill = this->fill();
    auto local_bounds = this->local_bounds();
    if (local_bounds.width == 0 || local_bounds.height == 0) {
        return;
    }

    Util::Size2f texture_size { fill.texture() ? fill.texture()->size().cast<float>() : Util::Size2f {} };
    auto texture_rect = fill.texture_rect();
    if (texture_rect.size().is_zero()) {
        texture_rect.width = texture_size.x();
        texture_rect.height = texture_size.y();
    }

    auto normalized_texture_coord_for_point = [&](Util::Point2f point) -> Util::Point2f {
        if (!fill.texture()) {
            return {};
        }

        Util::Point2f point_normalized_coords {
            (point.x() - local_bounds.left) / local_bounds.width,
            (point.y() - local_bounds.top) / local_bounds.height,
        };
        Util::Point2f texture_coords {
            texture_rect.left + point_normalized_coords.x() * texture_rect.width,
            texture_rect.top + point_normalized_coords.y() * texture_rect.height,
        };

        return { texture_coords.x() / texture_size.x(), texture_coords.y() / texture_size.y() };
    };

    std::vector<Gfx::Vertex> fill_vertices;
    fill_vertices.reserve(m_vertex_cache.size());
    for (auto const& point : m_vertex_cache) {
        fill_vertices.push_back(Gfx::Vertex {
            point,
            fill.color(),
            normalized_texture_coord_for_point(point),
        });
    }
    m_fill_vertex_cache = std::move(fill_vertices);
}

void Shape::ensure_cache_up_to_date() const {
    if (!m_vertex_cache_dirty) {
        return;
    }
    m_vertex_cache_dirty = false;

    if (outline().is_rounded()) {
        m_vertex_cache = calculate_vertices_for_rounded_shape(*this);
    }
    else {
        m_vertex_cache = points().to_vector();
    }

    generate_fill_vertices();
}

std::vector<Util::Point2f> const& Shape::vertices() const {
    ensure_cache_up_to_date();
    return m_vertex_cache;
}

std::vector<Gfx::Vertex> const& Shape::fill_vertices() const {
    ensure_cache_up_to_date();
    return m_fill_vertex_cache;
}
}
