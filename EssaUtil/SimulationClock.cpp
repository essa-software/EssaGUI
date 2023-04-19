#include "SimulationClock.hpp"

Util::SimulationClock::Format Util::SimulationClock::time_format = Util::SimulationClock::Format::SHORT_TIME;

std::ostream& operator<<(std::ostream& out, Util::SimulationClock::time_point const& time) {
    auto time_as_time_t = time.time_since_epoch().count();

    tm* time_as_tm = gmtime(&time_as_time_t);

    char buffer[256];

    switch (Util::SimulationClock::time_format) {
    case Util::SimulationClock::Format::SHORT_TIME:
        strftime(buffer, 256, "%D, %T", time_as_tm);
        break;
    case Util::SimulationClock::Format::AMERICAN:
        strftime(buffer, 256, "%F, %r", time_as_tm);
        break;
    case Util::SimulationClock::Format::MID_TIME:
        strftime(buffer, 256, "%a, %d %b %G, %T", time_as_tm);
        break;
    case Util::SimulationClock::Format::LONG_TIME:
        strftime(buffer, 256, "%A, %d %B %G, %T", time_as_tm);
        break;
    case Util::SimulationClock::Format::NO_CLOCK_SHORT:
        strftime(buffer, 256, "%D", time_as_tm);
        break;
    case Util::SimulationClock::Format::NO_CLOCK_AMERICAN:
        strftime(buffer, 256, "%F", time_as_tm);
        break;
    case Util::SimulationClock::Format::NO_CLOCK_MID:
        strftime(buffer, 256, "%a, %d %b %G", time_as_tm);
        break;
    case Util::SimulationClock::Format::NO_CLOCK_LONG:
        strftime(buffer, 256, "%A, %d %B %G", time_as_tm);
        break;
    }

    out << buffer;
    return out;
}

namespace Util::SimulationTime {

SimulationClock::time_point create(int year, int month, int day, int hour, int minute, int second) {
    tm time {
        .tm_sec = second,
        .tm_min = minute,
        .tm_hour = hour,
        .tm_mday = day,
        .tm_mon = month - 1,
        .tm_year = year - 1900,
        .tm_wday = 0,
        .tm_yday = 0,
        .tm_isdst = 0,
#ifdef __USE_MISC
        .tm_gmtoff = {},
        .tm_zone = {},
#endif
    };
    auto time_epoch = mktime(&time);
    return SimulationClock::time_point(SimulationClock::duration(time_epoch));
}

}
