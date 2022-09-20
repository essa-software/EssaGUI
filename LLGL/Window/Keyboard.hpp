#pragma once

#include <SDL2/SDL_keycode.h>
#include <string>

#include <SDL2/SDL_keyboard.h>

namespace llgl {

#define ENUMERATE_KEYBOARD_KEYCODES(K) \
    K(A, SDLK_a)                       \
    K(B, SDLK_b)                       \
    K(C, SDLK_c)                       \
    K(D, SDLK_d)                       \
    K(E, SDLK_e)                       \
    K(F, SDLK_f)                       \
    K(G, SDLK_g)                       \
    K(H, SDLK_h)                       \
    K(I, SDLK_i)                       \
    K(J, SDLK_j)                       \
    K(K, SDLK_k)                       \
    K(L, SDLK_l)                       \
    K(M, SDLK_m)                       \
    K(N, SDLK_n)                       \
    K(O, SDLK_o)                       \
    K(P, SDLK_p)                       \
    K(Q, SDLK_q)                       \
    K(R, SDLK_r)                       \
    K(S, SDLK_s)                       \
    K(T, SDLK_t)                       \
    K(U, SDLK_u)                       \
    K(V, SDLK_v)                       \
    K(W, SDLK_w)                       \
    K(X, SDLK_x)                       \
    K(Y, SDLK_y)                       \
    K(Z, SDLK_z)                       \
    K(Up, SDLK_UP)                     \
    K(Down, SDLK_DOWN)                 \
    K(Left, SDLK_LEFT)                 \
    K(Right, SDLK_RIGHT)               \
    K(Backspace, SDLK_BACKSPACE)       \
    K(Delete, SDLK_DELETE)             \
    K(Escape, SDLK_ESCAPE)             \
    K(End, SDLK_END)                   \
    K(Enter, SDLK_RETURN)              \
    K(Home, SDLK_HOME)                 \
    K(Space, SDLK_SPACE)               \
    K(LShift, SDLK_LSHIFT)             \
    K(RShift, SDLK_RSHIFT)             \
    K(Tab, SDLK_TAB)                   \
    K(Tilde, SDLK_BACKQUOTE)

enum class KeyCode {
#define __ENUMERATE_KEY(key, value) key = value,
    ENUMERATE_KEYBOARD_KEYCODES(__ENUMERATE_KEY)
#undef __ENUMERATE_KEY
};

constexpr std::string_view to_string(KeyCode key) {
    switch (key) {
#define __ENUMERATE_KEY(key, value) \
    case KeyCode::key:              \
        return #key;
        ENUMERATE_KEYBOARD_KEYCODES(__ENUMERATE_KEY)
#undef __ENUMERATE_KEY
    }
    return "Unknown";
}

bool is_key_pressed(KeyCode key);
inline bool is_shift_pressed() { return is_key_pressed(KeyCode::LShift) || is_key_pressed(KeyCode::RShift); }

}
