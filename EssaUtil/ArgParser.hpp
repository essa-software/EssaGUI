#pragma once

#include "Error.hpp"
#include <functional>
#include <map>

namespace Util {

class ArgParser {
public:
    using Result = Util::ErrorOr<void, std::string>;
    struct PositionalParameter {
        using Handler = std::function<Result(std::string_view argument)>;
        Handler handler;
        std::string_view name;
    };
    struct Option {
        using Handler = std::function<Result(std::string_view value)>;
        Handler handler;
        std::string_view name;
        bool is_boolean;
        std::string_view help_string {};
    };

    explicit ArgParser(int argc, char* argv[]);

    template<class T>
    void parameter(std::string_view name, T& target) {
        m_positional_parameters.push_back(parameter_handler(name, target));
    }

    template<class T>
    void parameter(std::string_view name, std::optional<T>& target) {
        m_optional_positional_parameters.push_back({
            .handler = [name, &target](std::string_view value) -> Result {
                T out;
                TRY(parameter_handler(name, out).handler(value));
                target = std::move(out);
                return {};
            },
            .name = name,
        });
    }

    template<class T>
    void option(std::string_view name, T& target, std::string_view help_string) {
        auto opt = option_handler(name, target);
        opt.help_string = help_string;
        m_options.insert({ name, std::move(opt) });
    }

    template<class T>
    void option(std::string_view name, std::optional<T>& target, std::string_view help_string) {
        m_options.insert({
            name,
            Option {
                .handler = [name, &target, help_string](std::string_view value) -> Result {
                    T out;
                    TRY(option_handler(name, out).handler(value));
                    target = std::move(out);
                    return {};
                },
                .name = name,
                .is_boolean = std::is_same_v<T, bool>,
                .help_string = help_string,
            },
        });
    }

    Result parse() const;
    void parse_or_quit() const;

    void display_help() const;

private:
    static ArgParser::PositionalParameter parameter_handler(std::string_view name, int& target);
    static ArgParser::PositionalParameter parameter_handler(std::string_view name, std::string& target);
    static ArgParser::Option option_handler(std::string_view name, std::string& target);
    static ArgParser::Option option_handler(std::string_view name, bool& target);

    std::vector<std::string_view> m_arguments;
    std::vector<PositionalParameter> m_positional_parameters;
    std::vector<PositionalParameter> m_optional_positional_parameters;
    std::map<std::string_view, Option> m_options;
    bool m_display_help = false;
};

}
