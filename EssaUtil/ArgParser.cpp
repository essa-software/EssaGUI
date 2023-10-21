#include "ArgParser.hpp"

#include <algorithm>
#include <fmt/format.h>

namespace Util {

ArgParser::PositionalParameter ArgParser::parameter_handler(std::string_view name, int& target) {
    return {
        .handler = [name, &target](std::string_view param) -> ArgParser::Result {
            try {
                target = std::stoi(std::string(param));
                return {};
            } catch (...) {
                return fmt::format("Failed to parse int for parameter '{}'", name);
            }
        },
        .name = name,
    };
}

ArgParser::PositionalParameter ArgParser::parameter_handler(std::string_view name, std::string& target) {
    return {
        .handler = [&target](std::string_view param) -> ArgParser::Result {
            target = param;
            return {};
        },
        .name = name,
    };
}

ArgParser::Option ArgParser::option_handler(std::string_view name, std::string& target) {
    return {
        .handler = [&target](std::string_view param) -> ArgParser::Result {
            target = param;
            return {};
        },
        .name = name,
        .is_boolean = false,
    };
}

ArgParser::Option ArgParser::option_handler(std::string_view name, bool& target) {
    return {
        .handler = [&target](std::string_view) -> ArgParser::Result {
            // Note: This handler will be called only if the option is given
            //       in arguments, so we can assume that it is true.
            target = true;
            return {};
        },
        .name = name,
        .is_boolean = true,
    };
}

ArgParser::ArgParser(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        m_arguments.push_back(std::string_view(argv[i]));
    }
}

ArgParser::Result ArgParser::parse() const {
    size_t positional_arg_offset = 0;
    for (size_t i = 1; i < m_arguments.size(); i++) {
        if (m_arguments[i].starts_with("-")) {
            auto it = m_options.find(m_arguments[i]);
            if (it == m_options.end()) {
                return fmt::format("Unknown option: '{}'", m_arguments[i]);
            }
            auto const& option = it->second;
            if (option.is_boolean) {
                TRY(it->second.handler(""));
            }
            else {
                i++;
                if (i >= m_arguments.size()) {
                    return fmt::format("'{}' requires an argument", option.name);
                }
                TRY(it->second.handler(m_arguments[i]));
            }
            continue;
        }

        auto const& param = *TRY([&]() -> Util::ErrorOr<PositionalParameter const*, std::string> {
            size_t idx = positional_arg_offset;
            if (idx < m_positional_parameters.size()) {
                return &m_positional_parameters[idx];
            }
            idx -= m_positional_parameters.size();
            if (idx < m_optional_positional_parameters.size()) {
                return &m_optional_positional_parameters[idx];
            }
            return "Unused parameter(s)";
        }());
        positional_arg_offset++;

        TRY(param.handler(m_arguments[i]));
    }
    if (positional_arg_offset < m_positional_parameters.size()) {
        std::vector<std::string_view> out;
        std::ranges::transform(m_positional_parameters, std::back_inserter(out), [](auto& param) { return param.name; });
        return fmt::format("Missing required parameters: {}", fmt::join(out, ", "));
    }
    return {};
}

}
