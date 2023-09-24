#include <EssaUtil/Testing.hpp>

#include <EssaUtil/UStringBuilder.hpp>
#include <algorithm>

TEST_CASE(append) {
    Util::UStringBuilder builder;
    builder.append("siema tej");
    builder.append("hello there");
    builder.append('\n');
    builder.appendff("{:.2f}", 1.2345);
    EXPECT_EQ(builder.build().encode(), "siema tejhello there\n1.23");
    return {};
}

TEST_CASE(reserve_release) {
    Util::UStringBuilder builder1;
    builder1.reserve(1000);
    for (size_t s = 0; s < 1000; s++) {
        builder1.append(' ');
    }
    auto string_template = builder1.release_string();

    Util::UStringBuilder builder2;
    builder2.reserve(1000'000);
    for (size_t s = 0; s < 1000; s++) {
        builder2.append(string_template);
    }

    auto string = builder2.release_string();

    EXPECT(std::find_if_not(string.begin(), string.end(), [](uint32_t cp) { return cp == ' '; }) == string.end());
    return {};
}
