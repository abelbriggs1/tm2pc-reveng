/**
 * @file winmain.c
 *
 * Win32 entry point.
 */

#include <windows.h>

#include "tm2.h"

#include "environment.h"

static HMODULE tm_lib;  // @address 0x00BDFB28

/**
 * Register a Win32 window class with the specified parameters.
 *
 * @address        0x004A6B80
 *
 * @param[in]      name                Name of the window class.
 * @param[in]      instance            Instance handle for the application.
 * @param[in]      style               Window style.
 * @param[in]      bg                  Window background.
 *
 * @return         ATOM                Handle for the window class.
 */
static ATOM RegisterWindowClass (LPCSTR name, HINSTANCE instance, UINT style, HBRUSH bg)
{
  WNDCLASSA wClass;

  wClass.style = style;
  wClass.hInstance = instance;
  wClass.lpfnWndProc = DefWindowProcA;
  wClass.cbClsExtra = NULL;
  wClass.cbWndExtra = NULL;
  wClass.hIcon = LoadIconA (instance, "GAME_ICON");
  wClass.hCursor = LoadCursorA (NULL, IDC_ARROW);
  wClass.hbrBackground = bg;
  wClass.lpszMenuName = NULL;
  wClass.lpszClassName = name;
  return RegisterClassA (&wClass);
}

/**
 * Win32 entry point. Sets up various Windows-specific operations, then jumps to
 * game initialization.
 *
 * @address    0x004A8B04
 *
 * @param[in]  instance            Handle to the instance for TM2.
 * @param[in]  prev_instance       Handle to the previous instance. Always NULL.
 * @param[in]  cmd_line            Command line parameters.
 * @param[in]  cmd_show            Default window show state.
 *
 * @return     int                 Return code.
 */
int WinMain (HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
  RECT rc;
  // Set up registry key "Software\Sony Entertainment\Twisted Metal 2\1.0.1"
  PHKEY reg_key;
  TmRegistryUseKey (NULL);
  TmRegistryInit ("Software\\Sony Interactive\\Twisted Metal 2\\1.0.1");
  TmRegistryCreateKey (&reg_key, HKEY_LOCAL_MACHINE, NULL);
  TmRegistryCloseKey (&reg_key);

  // Process arguments and get Display Capabilities (screen width, height, bits per pixel)
  Environment env;
  EnvironmentInit (&env);

  // Test if the game is running from a CD Drive - save the drive letter in a global variable if so
  TmFsVerifyCdDrive ("tm2.ico");

  TmComLoadLibrary (&tm_lib, "TM2English");

  TmMovieInit ();

  // Initialize Windows COM
  if (FAILED (CoInitialize (NULL))) {
    return 1;
  }

  // TODO: setjmp (0x00BDFAF0) is used here. From some research, this appears to be
  // MSVC structured exception handling (SEH), specifically a termination handler.
  // More research is needed.
  __try {
    // Detect CDROM, initialize paths to various resources from registry if possible
    int result = TmFsInit ("tm2.ico");
    if (!result) {
      return !result;
    }

    // Calculate main window size and location (default size 640x480)
    DWORD x = 48;
    DWORD y = 48;
    SetRect (&rc, 0, 0, 640, 480);
    AdjustWindowRect (&rc, WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
    TmRegistryOpenKey (&reg_key, HKEY_LOCAL_MACHINE, NULL);
    TmRegistryQueryKeyDword (&reg_key, "Window XPos", &x);
    TmRegistryQueryKeyDword (&reg_key, "Window YPos", &y);
    TmRegistryCloseKey (&reg_key);
    if (rc.right + x > env.screen_width) {
      x = env.screen_width - rc.right;
    }
    if (x < 0) {
      x = 0;
    }
    if (rc.bottom + y > env.screen_height) {
      y = env.screen_height - rc.bottom;
    }
    if (y < 0) {
      y = 0;
    }

    // Register window class with name "Twisted Metal 2 Class"
    RegisterWindowClass ("Twisted Metal 2 Class", instance, CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
                         NULL);

    // Initialize window with the expected class name, title bar name ("Twisted Metal 2"), position,
    // and size.
    TmWindowInit (WS_EX_APPWINDOW, "Twisted Metal 2 Class", "Twisted Metal 2",
                  WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, x, y, rc.right,
                  rc.bottom, NULL, NULL, instance);  // inlined
    TmWindowShow (cmd_show);                         // inlined

    // Boilerplate Win32 window setup
    RECT rect;
    HWND wnd = TmWindowGetHwnd ();  // inlined
    HDC caps = GetDC (wnd);
    GetClientRect (wnd, &rect);
    FillRect (caps, &rect, GetStockObject (BLACK_BRUSH));
    ReleaseDC (wnd, caps);

    // Jump to main
    main (EnvironmentGetNumArgs (&env), EnvironmentGetCmdArgs (&env));
  } __finally {
    // TODO: Unknown de-init function 0x004775A8 called here
    if (GetWindowRect (instance, &rc)) {
      TmRegistryOpenKey (&reg_key, HKEY_LOCAL_MACHINE, NULL);
      TmRegistrySetKeyDword (&reg_key, "Window XPos", rc.left);
      TmRegistrySetKeyDword (&reg_key, "Window YPos", rc.top);
      TmRegistryCloseKey (&reg_key);
    }
    TmWindowDeinit ();  // inlined
    ClipCursor (NULL);
    CoUninitialize ();
    TmComFreeLibrary (&tm_lib);
  }
  // TODO: Unknown how the return code is decided.
  return 0;
}
