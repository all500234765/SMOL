#include "Platform/Input.h"

void Input::Init() {

}

void Input::UpdateMouseDeriv(f32 X, f32 Y, bool relative) {
}

void Input::UpdateMouseState(u64 flags) {
}

void Input::UpdateMouseState(MouseButton w, bool Down) {
}

void Input::UpdateKeyboardSetState(u32 key, u32 flags) {

}

bool Input::IsPressed(MouseButton button) {
    return false;
}

bool Input::IsReleased(MouseButton button) {
    return false;
}

bool Input::IsDown(MouseButton button) {
    return false;
}

bool Input::IsPressed(KeyboardKeys key) {
    return false;
}

bool Input::IsReleased(KeyboardKeys key) {
    return false;
}

bool Input::IsDown(KeyboardKeys key) {
    return false;
}

bool Input::IsPressed(GamepadButton button) {
    return false;
}

bool Input::IsReleased(GamepadButton button) {
    return false;
}

bool Input::IsDown(GamepadButton button) {
    return false;
}
