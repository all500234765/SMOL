#include "Platform/Input.h"
#include "Misc/Utils.h"

#include <pspctrl.h>

static SceCtrlData data;
static SceCtrlLatch latch;

void Input::Init() {

}

void Input::UpdateMouseDeriv(f32 X, f32 Y, bool relative) {}
void Input::UpdateMouseState(u64 flags) {}
void Input::UpdateMouseState(MouseButton w, bool Down) {}
void Input::UpdateKeyboardSetState(u32 key, u32 flags) {}

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

s32 convert(GamepadButton button) {
    switch( button ) {
        case GamepadButton::A: return PSP_CTRL_CROSS;
        case GamepadButton::B: return PSP_CTRL_CIRCLE;
        case GamepadButton::X: return PSP_CTRL_TRIANGLE;
        case GamepadButton::Y: return PSP_CTRL_SQUARE;

        case GamepadButton::Back:  return PSP_CTRL_SELECT;
        case GamepadButton::Start: return PSP_CTRL_START;
        case GamepadButton::Left:  return PSP_CTRL_LEFT;
        case GamepadButton::Right: return PSP_CTRL_RIGHT;

        case GamepadButton::ShoulderL: return PSP_CTRL_LTRIGGER;
        case GamepadButton::ShoulderR: return PSP_CTRL_RTRIGGER;
    }

    return 0;
}

bool Input::IsPressed(GamepadButton button)  { return latch.uiPress & convert(button); }
bool Input::IsReleased(GamepadButton button) { return latch.uiBreak & convert(button); }
bool Input::IsDown(GamepadButton button)     { return latch.uiMake  & convert(button); }

void Input::UpdateA() {
    sceCtrlReadBufferPositive(&data, 1); // Poll pad
    sceCtrlReadLatch(&latch);            // Poll latch
}

void Input::UpdateB() {}
void Input::UpdateC() {}

f32 Input::GetJoystickX(u32 index) { return (ToF32(data.Lx) / 127.0f - 1.0f); }
f32 Input::GetJoystickY(u32 index) { return (ToF32(data.Ly) / 127.0f - 1.0f); }


