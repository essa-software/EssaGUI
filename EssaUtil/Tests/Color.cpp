#include <EssaUtil/Testing.hpp>

#include <EssaUtil/Color.hpp>

TEST_CASE(to_html_string) {
    EXPECT_EQ(Colors::White.to_html_string().encode(), "#ffffff");
    EXPECT_EQ(Colors::AliceBlue.to_html_string().encode(), "#f0f8ff");
    EXPECT_EQ(Color { 0x030303ff }.to_html_string().encode(), "#030303");
    return {};
}

TEST_CASE(blend) {
    EXPECT_EQ(Color::rgb_blend(Color { 0xff00ff00 }, Color { 0x00000000 }, 0.5), Color { 0x7f007f00 });
    EXPECT_EQ(Color::rgb_blend(Color { 0xff00ff00 }, Color { 0x00000000 }, -1), Color { 0xff00ff00 });
    EXPECT_EQ(Color::rgb_blend(Color { 0xff00ff00 }, Color { 0x00000000 }, 2), Color { 0x00000000 });

    // FIXME: How should we actually handle alpha?
    EXPECT_EQ(Color::rgb_blend(Color { 0xff00ffff }, Color { 0x00000000 }, 0.5), Color { 0x7f007fff });

    return {};
}
