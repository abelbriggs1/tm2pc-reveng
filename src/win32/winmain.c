/**
 * @file winmain.c
 *
 * Win32 entry point.
 */

#include <windows.h>

#include "com/com.h"
#include "environment/environment.h"
#include "fs/fs.h"
#include "movie/movie.h"
#include "registry/registry.h"
#include "window/window.h"

static HMODULE tm_lib;   // @ 0x00BDFB28
static TmWindow window;  // @ 0x00BDFCD0

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
  // Set up registry key "Software\Sony Entertainment\Twisted Metal 2\1.0.1"
  PHKEY reg_key;
  TmRegistryInit ("Software\\Sony Interactive\\Twisted Metal 2\\1.0.1");
  TmRegistryCreateKey (&reg_key, HKEY_LOCAL_MACHINE, NULL);
  TmRegistryCloseKey (&reg_key);

  // Process arguments and get Display Capabilities (screen width, height, bits per pixel)
  TmEnvironment env;
  TmEnvironmentInit (&env);  // @ 0x004C45C0

  // Test if the game is running from a CD Drive - save the drive letter in a global variable if so
  TmFsGetCdDrive ("tm2.ico");

  TmComLoadLibrary (&tm_lib, "TM2English");

  // Open AVI video with the Windows MCI API
  TmMovieInit ();

  // Initialize Windows COM
  if (CoInitialize (NULL) < 0) {
    return 1;
  }

  // TODO: Is this Win32 structured exception handling?
  if (!setjmp ((jmp_buf*)0x00BDFAF0)) {
    // Detect CDROM, initialize paths to various resources from registry if possible
    int result = TmFsInit ("tm2.ico");
    if (!result) {
      return !result;
    }

    // Calculate main window size and location (default size 640x480)
    RECT rc;
    int x = 48;
    int y = 48;
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
    TmWindowInit (&window, WS_EX_APPWINDOW, "Twisted Metal 2 Class", "Twisted Metal 2",
                  WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, x, y, rc.right,
                  rc.bottom, NULL, NULL, instance);
    TmWindowShow (&window, cmd_show);

    // Boilerplate Win32 window setup
    RECT rect;
    HWND wnd = TmWindowGetHwnd (&window);
    HDC caps = GetDC (wnd);
    GetClientRect (wnd, &rect);
    FillRect (caps, &rect, GetStockObject (BLACK_BRUSH));
    ReleaseDC (wnd, caps);

    // Jump to main
    main (envGetNumArgs (&env), envGetCmdArgs (&env));  // @ 0x00477124
  }
  // TODO: Various de-init stuff if WinMain returns
}
