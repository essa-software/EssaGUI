#include "Essa/GUI/EML/Formatter.hpp"
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/HostWindow.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/WindowRoot.hpp>
#include <EssaUtil/Stream/File.hpp>
#include <filesystem>
#include <gtest/gtest.h>
#include <regex>

struct TestCase {
    std::string test_name;
    std::string eml_input;
    std::string expected_formatted_eml;
};
std::ostream& operator<<(std::ostream& out, TestCase const& ts) {
    return out << "TestCase(" << ts.test_name << ")";
}

class EMLTest : public testing::TestWithParam<TestCase> { };

TEST_P(EMLTest, EMLTest) {
    auto const& p = GetParam();

    std::ostringstream oss;
    if (p.expected_formatted_eml.empty()) {
        fmt::print(stderr, "\e[31mNo expectation for test {}, printing actual output:\e[m\n", p.test_name);
        FAIL() << "No expectation for test";
    }

    EML::Formatter formatter(p.eml_input);
    auto formatted_eml = formatter.format();
    EXPECT_EQ(formatted_eml, p.expected_formatted_eml);
}

namespace re {

std::optional<std::smatch> match(std::regex const& pattern, std::string const& string) {
    std::smatch match;
    bool matched = std::regex_search(string, match, pattern);
    return matched ? match : std::optional<std::smatch>();
}

}

std::vector<TestCase> read_test_cases() {
    std::vector<TestCase> tests;
    for (auto const& file : std::filesystem::recursive_directory_iterator(TESTCASES_PATH)) {
        if (file.path().extension().string() == ".eml") {
            auto stem = file.path();
            stem.replace_extension("");
            auto input_file = MUST(Util::ReadableFileStream::read_file(stem.replace_extension(".eml")));

            auto expectation_file = MUST(Util::ReadableFileStream::read_file(stem.replace_extension(".expected_eml")));
            tests.push_back(TestCase {
                .test_name = file.path().lexically_relative(TESTCASES_PATH).string(),
                .eml_input = MUST(input_file.decode()).encode(),
                .expected_formatted_eml = MUST(expectation_file.decode()).encode(),
            });
        }
    }
    return tests;
}

INSTANTIATE_TEST_CASE_P(
    GeneralAndSpecial, // Instantiation name can be chosen arbitrarily.
    EMLTest, testing::ValuesIn(read_test_cases())
);

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
