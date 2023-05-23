#include "Clipboard.hpp"

#include <SDL2/SDL_clipboard.h>
#include <cassert>

namespace llgl {

Util::UString Clipboard::get_string() {
    if (!has_string())
        return "";
    auto text = SDL_GetClipboardText();
    assert(text);
    Util::UString str { text };
    SDL_free(text);
    return str;
}

void Clipboard::set_string(Util::UString const& string) { SDL_SetClipboardText(string.encode().c_str()); }

bool Clipboard::has_string() { return SDL_HasClipboardText(); }

}
