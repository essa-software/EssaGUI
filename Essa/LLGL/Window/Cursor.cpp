#include "Cursor.hpp"

#include <EssaUtil/DelayedInit.hpp>
#include <SDL2/SDL_mouse.h>
#include <fmt/core.h>
#include <fmt/format.h>

namespace llgl {

Cursor const& Cursor::system(SystemCursor cursor) {
    static Util::DelayedInit<Cursor> cursors[static_cast<int>(SystemCursor::Count)];
    static bool initialized = false;
    if (!initialized) {
        for (int i = 0; i < static_cast<int>(SystemCursor::Count); ++i) {
            cursors[i].construct(SystemCursor(i));
        }
        initialized = true;
    }
    return *cursors[static_cast<int>(cursor)];
}

Cursor::~Cursor() {
    if (m_cursor) {
        fmt::print("free cursor {}\n", fmt::ptr(m_cursor));
        SDL_FreeCursor(m_cursor);
    }
}

Cursor::Cursor(SystemCursor cursor) {
    m_cursor = SDL_CreateSystemCursor((SDL_SystemCursor)cursor);
    if (!m_cursor) {
        fmt::print("failed to create system cursor: {}\n", SDL_GetError());
    }
}

void Cursor::set(Cursor const& cursor) {
    SDL_SetCursor(cursor.m_cursor);
}

void Cursor::show(bool show) {
    // SDL_ENABLE/SDL_DISABLE doesn't exist???
    SDL_ShowCursor(show);
}

}
