#include "pc.h"

#include "Platform/Window.h"
#include "Platform/Input.h"

#include "Misc/Log.h"
#include "Misc/StringUtils.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

LRESULT InternalProcess(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

HWND m_hwnd;
int position_x, position_y;

void Window::Init() {
    const wchar_t* class_name = L"WinWindow::Class";

    WNDCLASSEX wnd_class{};
    wnd_class.lpfnWndProc = &InternalProcess;
    wnd_class.lpszClassName = class_name;
    wnd_class.hInstance = GetModuleHandle(NULL);
    wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wnd_class.cbClsExtra = 0;
    wnd_class.cbWndExtra = 0;
    wnd_class.hIcon = NULL;
    wnd_class.hIconSm = wnd_class.hIcon;
    wnd_class.hCursor = NULL;
    wnd_class.hbrBackground = NULL;
    wnd_class.lpszMenuName = NULL;
    wnd_class.cbSize = sizeof(WNDCLASSEX);

    if( !RegisterClassEx(&wnd_class) )
        Log::Crash("WinWindow::Create(): Failed to register a class %d\n", GetLastError());

    // Window flags
    u32 flags = 0;

    //if( config.Borderless ) flags |= WS_POPUP;
    //else
    flags |= WS_OVERLAPPEDWINDOW;


    u32 screen_width = GetSystemMetrics(SM_CXSCREEN);
    u32 screen_height = GetSystemMetrics(SM_CYSCREEN);

    if( IsWindowed() ) {
        // Place the window in the middle of the screen.
        position_x = std::max((screen_width - width) / 2, 0u);
        position_y = std::max((screen_height - height) / 2, 0u);
    } else {
        // If full screen set the screen to maximum size of the users desktop and 32bit.
        //memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        //dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
        //dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
        //dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        //dmScreenSettings.dmBitsPerPel = 32;
        //dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        //
        //if( !config.Borderless ) {
        //    // Change the display settings to full screen.
        //    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
        //}
        //
        //cfg.CurrentWidth  = screenWidth;
        //cfg.CurrentHeight = screenHeight;
        //
        //// Set the position of the window to the top left corner.
        //posX = posY = 0;
    }

    RECT rect = { position_x, position_y, position_x + (int)width, position_y + (int)height };
    AdjustWindowRectEx(&rect, flags, false, WS_EX_APPWINDOW);

    position_x = (int)rect.left;
    position_y = (int)rect.top;
    width = (u32)(rect.right - rect.left);
    height = (u32)(rect.bottom - rect.top);

    StringW wtitle = StringWiden("Do something already!");

    windowed = true;
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, wnd_class.lpszClassName, wtitle.c_str(), flags,
                            position_x, position_y, width, height,
                            NULL, NULL, wnd_class.hInstance, NULL);

    if( !m_hwnd )
        Log::Crash("WinWindow::Create(): Failed to create a window %d\n", GetLastError());
    
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
}

void* Window::GetHandle() { return m_hwnd; }

LRESULT InternalProcess(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch( msg ) {
        case WM_DESTROY:
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        case WM_INPUT:
        {
            UINT dwSize;
            GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
            LPBYTE lpb = new BYTE[dwSize];
            if( lpb == NULL )
                return 0;


            if( GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize )
                //OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));
                Log::Warning("WinWindow::InternalProcess(): GetRawInputData does not return correct size\n");

            RAWINPUT* raw = (RAWINPUT*)lpb;

            if( raw->header.dwType == RIM_TYPEMOUSE ) {
                // Update mouse position
                POINT lp;
                GetCursorPos(&lp);
                ScreenToClient(hwnd, &lp);

                Input::UpdateMouseDeriv((float)lp.x, (float)lp.y, false);

                // Update buttons
                if( raw->data.mouse.ulButtons > (ULONG)0 )
                    Input::UpdateMouseState(raw->data.mouse.ulButtons);
            }

            if( raw->header.dwType == RIM_TYPEKEYBOARD ) {
                Input::UpdateKeyboardSetState(raw->data.keyboard.VKey, !(raw->data.keyboard.Flags & RI_KEY_BREAK));
            }
        }
        break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}