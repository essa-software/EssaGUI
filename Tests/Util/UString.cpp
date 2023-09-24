#include <EssaUtil/Testing.hpp>

#include <EssaUtil/UString.hpp>
#include <algorithm>
#include <vector>

TEST_CASE(construction) {
    // Default constructor
    UString str1;
    EXPECT(str1.size() == 0);
    EXPECT(str1.storage() == nullptr);

    // 1-char constructor
    UString str2 { 'a' };
    EXPECT(str2.size() == 1);
    EXPECT(str2.at(0) == 'a');

    // 1-char constructor with unicode character
    UString str3 { L'ą' };
    EXPECT(str3.size() == 1);
    EXPECT(str3.at(0) == L'ą');

    // Constructor from std::span
    uint8_t arr[] = { 'x', 'd', 'l', 'o', 'l' };
    UString str4 { arr };
    EXPECT(str4 == "xdlol");

    // Constructor from empty string
    UString str5 { std::string_view("") };
    EXPECT(str5.is_empty());

    return {};
}

TEST_CASE(copy) {
    // Copy constructor
    UString str1 { "abcdef" };
    auto str2 = str1;
    EXPECT(str2 == str1);

    // Copy assignment
    UString str3 { "ghijkl" };
    str2 = str3;
    EXPECT(str2 == str3);

    return {};
}

TEST_CASE(move) {
    // Move constructor
    UString str1 { "abcdef" };
    auto str2 = std::move(str1);
    EXPECT(str2 == "abcdef");
    EXPECT(str1.is_empty());

    // Move assignment
    UString str3 { "ghijkl" };
    str2 = std::move(str3);
    EXPECT(str2 == "ghijkl");
    EXPECT(str3.is_empty());

    // Move assignment (double-free on move-assigning empty string)
    {
        UString str1 { "abc" };
        str1 = "";    // str1 should be null here
        str1 = "def"; // it was double-freed because we didn't clear the pointer to "abc"
        EXPECT_EQ(str1.encode(), "def");
    }

    return {};
}

TEST_CASE(utf8) {
    struct Testcase {
        char const* string;
        std::vector<uint32_t> expected;
    };

    Testcase testcases[] = { { "abcdef", { 'a', 'b', 'c', 'd', 'e', 'f' } },
                             { "ąęłπę", { L'ą', L'ę', L'ł', L'π', L'ę' } },
                             { "test你能？abceąę", { 't', 'e', 's', 't', L'你', L'能', L'？', L'a', L'b', L'c', L'e', L'ą', L'ę' } },
                             { "😀 😌 😔 😑 😆 😞", { L'😀', ' ', L'😌', ' ', L'😔', ' ', L'😑', ' ', L'😆', ' ', L'😞' } } };

    for (auto const& testcase : testcases) {
        // Decoding
        UString string { testcase.string };
        EXPECT(std::equal(string.storage(), string.storage() + string.size(), testcase.expected.data()));

        // Encoding
        auto string_in_utf8 = string.encode();
        EXPECT(std::equal(string_in_utf8.begin(), string_in_utf8.end(), testcase.string));
    }

    return {};
}

TEST_CASE(utf8_invalid) {
    std::vector<uint8_t> testcases[] { { 0xc4 } };

    for (auto const& testcase : testcases) {
        EXPECT(UString::decode(testcase).is_error());
    }

    return {};
}

BENCHMARK(utf8) {
    std::string big_string;
    constexpr size_t BigStringSize = 500000;
    big_string.resize(BigStringSize);
    for (size_t s = 0; s < BigStringSize; s++) {
        big_string[s] = s % 128;
    }
    (void)Util::UString { big_string }.encode();
}

TEST_CASE(concatenate) {
    UString str1 { "abc" };
    UString str2 { "ąęł" };
    EXPECT_EQ((str1 + str2).encode(), "abcąęł");

    return {};
}

TEST_CASE(substr) {
    UString big_string { "abcdefghijklmnopqrstuvwxyz" };
    EXPECT_EQ(big_string.substring(0, 3).encode(), "abc");
    EXPECT_EQ(big_string.substring(3, 6).encode(), "defghi");
    EXPECT_EQ(big_string.substring(23, 3).encode(), "xyz");
    EXPECT_EQ(big_string.substring(5).encode(), "fghijklmnopqrstuvwxyz");

    return {};
}

TEST_CASE(find) {
    UString haystack { "abcdefghijklmnopqrstuvwxyz" };
    EXPECT_EQ(haystack.find("abc").value(), 0ull);
    EXPECT_EQ(haystack.find("def").value(), 3ull);
    EXPECT_EQ(haystack.find("xyz").value(), 23ull);
    EXPECT_EQ(haystack.find("zyx").has_value(), false);
    EXPECT_EQ(haystack.find("abcdefghijklmnopqrstuvwxyz").value(), 0ull);
    EXPECT_EQ(haystack.find("abcdefghijklmnopqrstuvwxyza").has_value(), false);

    // Empty string
    EXPECT_EQ(UString { "" }.find("a").has_value(), false);

    return {};
}

TEST_CASE(find_one_of) {
    UString haystack { "abcabcabc" };
    EXPECT_EQ(haystack.find_one_of({ 'a' }, 0), 0);
    EXPECT_EQ(haystack.find_one_of({ 'b' }, 0), 1);
    EXPECT_EQ(haystack.find_one_of({ 'a' }, 3), 3);
    EXPECT_EQ(haystack.find_one_of({ 'b' }, 3), 4);
    EXPECT_EQ(haystack.find_one_of({ 'z' }, 0), std::optional<size_t>());

    return {};
}

TEST_CASE(erase) {
    UString test { "abcdefghij" };
    EXPECT_EQ(test.erase(0).encode(), "bcdefghij");
    EXPECT_EQ(test.erase(5).encode(), "abcdeghij");
    EXPECT_EQ(test.erase(9).encode(), "abcdefghi");

    EXPECT_EQ(test.erase(0, 2).encode(), "cdefghij");
    EXPECT_EQ(test.erase(5, 2).encode(), "abcdehij");
    EXPECT_EQ(test.erase(8, 2).encode(), "abcdefgh");

    // Out of bounds
    EXPECT_EQ(test.erase(8, 20).encode(), "abcdefgh");

    return {};
}

TEST_CASE(insert) {
    UString test { "abcghi" };
    EXPECT_EQ(test.insert("def", 3).encode(), "abcdefghi");
    EXPECT_EQ(test.insert("def", 0).encode(), "defabcghi");
    EXPECT_EQ(test.insert("def", 6).encode(), "abcghidef");

    return {};
}

TEST_CASE(starts_with) {
    UString test { "abcghi" };
    EXPECT(test.starts_with("a"));
    EXPECT(test.starts_with("abc"));
    EXPECT(test.starts_with("abcghi"));
    EXPECT(!test.starts_with("abcghij"));
    EXPECT(!test.starts_with("def"));
    EXPECT(!test.starts_with("abcdef"));

    return {};
}

TEST_CASE(indent) {
    EXPECT_EQ(UString { "test" }.indent(), 0ull);
    EXPECT_EQ(UString { "    test" }.indent(), 4ull);
    EXPECT_EQ(UString { "    " }.indent(), 4ull);

    return {};
}

TEST_CASE(for_each_split) {

    bool failed = false;
    size_t index = 0;

    {
        UString test1 { "line long " };
        test1.for_each_split(" ", [&](std::span<uint32_t const> span) {
            if (index == 0 && UString { span } != "line")
                failed = true;
            if (index == 1 && UString { span } != "long")
                failed = true;
            index++;
        });
        EXPECT(!failed && index == 2);
    }

    {
        index = 0;
        UString test2 { "line\nlong" };
        test2.for_each_line([&](std::span<uint32_t const> span) {
            if (index == 0 && UString { span } != "line")
                failed = true;
            if (index == 1 && UString { span } != "long")
                failed = true;
            index++;
        });
        EXPECT(!failed && index == 2);
    }

    {
        index = 0;
        UString test3 { "1" };
        test3.for_each_line([&](std::span<uint32_t const> span) {
            if (index == 0 && UString { span } != "1")
                failed = true;
            index++;
        });
        EXPECT(!failed && index == 1);
    }

    {
        index = 0;
        UString test4 { "siema\n\ntej\ntest" };
        test4.for_each_line([&](std::span<uint32_t const> span) {
            if (index == 0 && UString { span } != "siema")
                failed = true;
            if (index == 1 && UString { span } != "")
                failed = true;
            if (index == 2 && UString { span } != "tej")
                failed = true;
            if (index == 3 && UString { span } != "test")
                failed = true;
            index++;
        });
        EXPECT(!failed && index == 4);
    }

    return {};
}

TEST_CASE(parse) {
    UString in1 { "-1234" };
    EXPECT_EQ(in1.parse<int>().release_value(), -1234);
    EXPECT_EQ(in1.parse<long>().release_value(), -1234);
    EXPECT_EQ(in1.parse<long long>().release_value(), -1234);

    UString in2 { "1234" };
    EXPECT_EQ(in2.parse<unsigned long>().release_value(), 1234ul);
    EXPECT_EQ(in2.parse<unsigned long long>().release_value(), 1234ull);

    UString in3 { "123.375" };
    EXPECT_EQ(in3.parse<float>().release_value(), 123.375);
    EXPECT_EQ(in3.parse<double>().release_value(), 123.375);
    EXPECT_EQ(in3.parse<long double>().release_value(), 123.375);
    return {};
}
