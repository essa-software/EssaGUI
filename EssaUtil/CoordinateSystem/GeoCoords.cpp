#include "GeoCoords.hpp"

// M_PIf is a GCC extension, define it for other compilers
#ifndef M_PIf
#    define M_PIf ((float)M_PI)
#endif

namespace Util {

GeoCoords GeoCoords::normalized() const {
    // Start with removing turnovers
    float lat = std::fmod(m_latitude.rad(), M_PIf * 2);
    float lon = m_longitude.rad();

    // If we are in quarters II/III, we need to add 180deg to longitude because we are now on another hemisphere
    if (std::abs(lat) > M_PI / 2 && std::abs(lat) < 3 * M_PI / 2) {
        lon += M_PI;
        if (lat > 0) {
            lat = M_PI - lat;
        }
        else {
            lat = -M_PIf - lat;
        }
    }
    lat = std::remainder(lat, M_PIf); // For quarter IV
    lon = std::remainder(lon, M_PIf * 2);

    return { Angle::radians(lat), Angle::radians(lon) };
}

bool GeoCoords::is_approximately_equal(GeoCoords const& other, float epsilon) const {
    return std::abs((m_latitude - other.m_latitude).rad()) < epsilon && std::abs((m_longitude - other.m_longitude).rad()) < epsilon;
}

}
