#include <EssaUtil/Testing.hpp>

#include <EssaUtil/Base64.hpp>

// This is third-party code, let's assume it's well tested
// and add only a simple test to ensure it's working after
// we've made changes to it.

TEST_CASE(base64_encode) {
    auto const input = Util::Buffer { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc };
    auto const encoded = Util::Base64::encode(input);
    auto const decoded = Util::Base64::decode(encoded);
    EXPECT_EQ(input, decoded);
    return {};
}
