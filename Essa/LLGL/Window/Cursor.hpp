#pragma once

#include <EssaUtil/DelayedInit.hpp>
#include <SDL2/SDL_mouse.h>

namespace llgl {

enum class SystemCursor {
    Arrow = SDL_SYSTEM_CURSOR_ARROW,
    IBeam = SDL_SYSTEM_CURSOR_IBEAM,
    Wait = SDL_SYSTEM_CURSOR_WAIT,
    Crosshair = SDL_SYSTEM_CURSOR_CROSSHAIR,
    WaitArrow = SDL_SYSTEM_CURSOR_WAITARROW,
    SizeNWSE = SDL_SYSTEM_CURSOR_SIZENWSE,
    SizeNESW = SDL_SYSTEM_CURSOR_SIZENESW,
    SizeWE = SDL_SYSTEM_CURSOR_SIZEWE,
    SizeNS = SDL_SYSTEM_CURSOR_SIZENS,
    SizeAll = SDL_SYSTEM_CURSOR_SIZEALL,
    No = SDL_SYSTEM_CURSOR_NO,
    Hand = SDL_SYSTEM_CURSOR_HAND,
    Count = SDL_NUM_SYSTEM_CURSORS
};

// A simple wrapper around SDL_Cursor. Note that the cursor object MUST
// be kept alive for the duration of the cursor being set.
class Cursor {
public:
    Cursor(Cursor const&) = delete;
    Cursor(Cursor&& other) noexcept
        : m_cursor(other.m_cursor) {
        other.m_cursor = nullptr;
    }

    static Cursor const& system(SystemCursor cursor);
    static Cursor const& arrow() {
        return system(SystemCursor::Arrow);
    }
    static Cursor const& i_beam() {
        return system(SystemCursor::IBeam);
    }
    static Cursor const& wait() {
        return system(SystemCursor::Wait);
    }
    static Cursor const& crosshair() {
        return system(SystemCursor::Crosshair);
    }
    static Cursor const& wait_arrow() {
        return system(SystemCursor::WaitArrow);
    }
    static Cursor const& size_nwse() {
        return system(SystemCursor::SizeNWSE);
    }
    static Cursor const& size_nesw() {
        return system(SystemCursor::SizeNESW);
    }
    static Cursor const& size_we() {
        return system(SystemCursor::SizeWE);
    }
    static Cursor const& size_ns() {
        return system(SystemCursor::SizeNS);
    }
    static Cursor const& size_all() {
        return system(SystemCursor::SizeAll);
    }
    static Cursor const& no() {
        return system(SystemCursor::No);
    }
    static Cursor const& hand() {
        return system(SystemCursor::Hand);
    }

    ~Cursor();

    // TODO: Custom cursors

    static void set(Cursor const& cursor);
    static void show(bool show = true);

private:
    friend class Util::DelayedInit<Cursor>;
    Cursor(SystemCursor cursor);

    SDL_Cursor* m_cursor = nullptr;
};

}
