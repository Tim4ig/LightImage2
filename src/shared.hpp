
#pragma once

#include <stdexcept>
#include <string>
#include <stacktrace>
#include <sstream>
#include "logger.hpp"

#define THROW(x) { throw std::runtime_error(std::string("Runtime error: ") + x + "\nStacktrace:\n" + get_stacktrace()); }
#define TIF ;{ if(FAILED(hr)) THROW(std::string("HRESULT was: ") + std::to_string((unsigned)hr)); }


inline std::string get_stacktrace() {
    std::ostringstream oss;
    const auto stack = std::stacktrace::current();

    for (std::size_t i = 1; i < stack.size(); ++i)
    {
        oss << stack[i].description() << '\n';
    }

    return oss.str();
}
