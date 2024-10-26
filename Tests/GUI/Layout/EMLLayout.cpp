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
    Util::Size2i wtr_size;
};
std::ostream& operator<<(std::ostream& out, TestCase const& ts) {
    return out << "TestCase(" << ts.test_name << ")";
}

class EMLTest : public testing::TestWithParam<TestCase> {
public:
    static void SetUpTestSuite() {
        app.construct();
    }
    static void TearDownTestSuite() {
        app.destruct();
    }

protected:
    static Util::DelayedInit<GUI::Application> app;
};

Util::DelayedInit<GUI::Application> EMLTest::app;

TEST_P(EMLTest, EMLTest) {
    auto const& p = GetParam();

    class MockWTR : public GUI::WidgetTreeRoot {
    public:
        explicit MockWTR(TestCase const& testcase)
            : m_testcase(testcase) { }
        virtual void setup(Util::UString, Util::Size2u, llgl::WindowSettings const&) override { }
        virtual void set_size(Util::Size2i) override { }
        virtual void center_on_screen() override { }
        virtual void close() override { }
        virtual GUI::HostWindow& host_window() override {
            static GUI::HostWindow hw;
            return hw;
        }
        virtual Util::Point2i host_position() const override {
            return {};
        }
        virtual Util::Size2i size() const override {
            return m_testcase.wtr_size;
        }

    private:
        TestCase const& m_testcase;
    };
    MockWTR wtr(p);
    GUI::WindowRoot root(wtr);

    auto& main_widget = root.set_main_widget<GUI::Container>();
    EXPECT_TRUE(main_widget.load_from_eml_string(p.eml_input)) << p.test_name << " failed to load";

    // copied from WindowRoot::relayout_and_draw()
    main_widget.set_size({ { static_cast<float>(wtr.size().x()), Util::Length::Px },
                           { static_cast<float>(wtr.size().y()), Util::Length::Px } });
    main_widget.set_raw_size(wtr.size());
    main_widget.do_relayout();

    std::ostringstream oss;
    main_widget.dump(oss, 0);
    if (p.expected_formatted_eml.empty()) {
        fmt::print(stderr, "\e[31mNo expectation for test {}, printing actual output:\e[m\n", p.test_name);
        main_widget.dump(std::cerr, 0);
        FAIL() << "No expectation for test";
    }
    else {
        EXPECT_EQ(oss.str(), p.expected_formatted_eml);
    }
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
            fmt::print("{}\n", stem.string());
            auto input_file = MUST(Util::ReadableFileStream::read_file(stem.replace_extension(".eml")));

            // Read size
            auto input_file_str = MUST(input_file.decode());
            auto nl_pos = input_file_str.find("\n");
            auto first_line = (nl_pos ? input_file_str.substring(0, *nl_pos) : input_file_str).encode();

            std::smatch match;
            auto matched = std::regex_search(first_line, match, std::regex("// (\\d*)x(\\d*)"));
            Util::Size2i wtr_size = matched && (match.size() == 3) ? Util::Size2i(std::stoi(match[1].str()), std::stoi(match[2].str()))
                                                                   : Util::Size2i(512, 512);

            auto expectation_file = MUST(Util::ReadableFileStream::read_file(stem.replace_extension(".txt")));
            tests.push_back(TestCase {
                .test_name = file.path().lexically_relative(TESTCASES_PATH).string(),
                .eml_input = MUST(input_file.decode()).encode(),
                .expected_formatted_eml = MUST(expectation_file.decode()).encode(),
                .wtr_size = wtr_size,
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
