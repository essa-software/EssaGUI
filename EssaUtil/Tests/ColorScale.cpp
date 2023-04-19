#include <EssaUtil/Testing.hpp>

#include <EssaUtil/ColorScale.hpp>

TEST_CASE(get) {
    ColorScale scale {
        { 0, Util::Colors::Red },
        { 0.5, Util::Colors::Lime },
        { 1, Util::Colors::Blue },
    };

    EXPECT_EQ(scale.get(-1), Util::Colors::Red);
    EXPECT_EQ(scale.get(0), Util::Colors::Red);
    EXPECT_EQ(scale.get(0.25), Util::Color { 0x7f7f00ff });
    EXPECT_EQ(scale.get(0.5), Util::Colors::Lime);
    EXPECT_EQ(scale.get(0.75), Util::Color { 0x007f7fff });
    EXPECT_EQ(scale.get(1), Util::Colors::Blue);
    EXPECT_EQ(scale.get(2), Util::Colors::Blue);

    return {};
}
