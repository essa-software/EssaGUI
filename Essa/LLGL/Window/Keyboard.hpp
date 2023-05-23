#pragma once

#include <EssaUtil/Enum.hpp>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <fmt/format.h>
#include <string>

namespace llgl {

// clang-format off
#define ENUMERATE_KEYBOARD_KEYCODES(Ex) \
    Ex(A, SDLK_a)                       \
    Ex(B, SDLK_b)                       \
    Ex(C, SDLK_c)                       \
    Ex(D, SDLK_d)                       \
    Ex(E, SDLK_e)                       \
    Ex(F, SDLK_f)                       \
    Ex(G, SDLK_g)                       \
    Ex(H, SDLK_h)                       \
    Ex(I, SDLK_i)                       \
    Ex(J, SDLK_j)                       \
    Ex(K, SDLK_k)                       \
    Ex(L, SDLK_l)                       \
    Ex(M, SDLK_m)                       \
    Ex(N, SDLK_n)                       \
    Ex(O, SDLK_o)                       \
    Ex(P, SDLK_p)                       \
    Ex(Q, SDLK_q)                       \
    Ex(R, SDLK_r)                       \
    Ex(S, SDLK_s)                       \
    Ex(T, SDLK_t)                       \
    Ex(U, SDLK_u)                       \
    Ex(V, SDLK_v)                       \
    Ex(W, SDLK_w)                       \
    Ex(X, SDLK_x)                       \
    Ex(Y, SDLK_y)                       \
    Ex(Z, SDLK_z)                       \
    Ex(Up, SDLK_UP)                     \
    Ex(Down, SDLK_DOWN)                 \
    Ex(Left, SDLK_LEFT)                 \
    Ex(Right, SDLK_RIGHT)               \
    Ex(Backspace, SDLK_BACKSPACE)       \
    Ex(Delete, SDLK_DELETE)             \
    Ex(Escape, SDLK_ESCAPE)             \
    Ex(End, SDLK_END)                   \
    Ex(Enter, SDLK_RETURN)              \
    Ex(Home, SDLK_HOME)                 \
    Ex(Space, SDLK_SPACE)               \
    Ex(LShift, SDLK_LSHIFT)             \
    Ex(RShift, SDLK_RSHIFT)             \
    Ex(Tab, SDLK_TAB)                   \
    Ex(Tilde, SDLK_BACKQUOTE)           \
    Ex(Period, SDLK_PERIOD)             \
    Ex(Comma, SDLK_COMMA) // clang-format on

ESSA_ENUM_WITH_VALUES(KeyCode, ENUMERATE_KEYBOARD_KEYCODES)
ESSA_ENUM_TO_STRING(KeyCode, ENUMERATE_KEYBOARD_KEYCODES)

bool is_key_pressed(KeyCode key);
inline bool is_shift_pressed() { return is_key_pressed(KeyCode::LShift) || is_key_pressed(KeyCode::RShift); }

}

ESSA_ENUM_FMT_FORMATTER(llgl, KeyCode, ENUMERATE_KEYBOARD_KEYCODES)
