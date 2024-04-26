#pragma once

#include <EssaUtil/Error.hpp>
#include <string>

namespace EML {

struct EMLError {
    std::string message;
};

template<class T>
using EMLErrorOr = Util::ErrorOr<T, EMLError>;

}
