/**
 * @file window.c
 *
 * Win32 window handling functions.
 */

#include <windows.h>

#include "win32/window.h"

static HINSTANCE cur_instance;  // @address 0x00BDFAE8

/**
 * Initialize a TmWindow with the specified parameters.
 *
 * @address        0x004A6BE8
 *
 * @param[in,out]  window              Window context.
 * @param[in]      ex_style            Extended window style. See Win32 documentation for
 *                                     CreateWindowExA().
 * @param[in]      class_name          Name of a registered window class. Optional.
 * @param[in]      window_name         Title bar string for the window. Optional.
 * @param[in]      style               Window style. See Win32 documentation for CreateWindowExA().
 * @param[in]      x                   Initial horizontal position of the window, in screen
 *                                     coordinates.
 * @param[in]      y                   Initial vertical position of the window, in screen
 *                                     coordinates.
 * @param[in]      width               Width of the window in device units.
 * @param[in]      height              Height of the window in device units.
 * @param[in]      parent              Handle to a parent window. Optional.
 * @param[in]      menu_name           Name of a menu to use with the window. Optional.
 * @param[in]      instance            Handle to an instance to be associated with the window.
 *                                     Optional.
 */
void TmWindowInit (TmWindow* window,
                   DWORD ex_style,
                   LPCSTR class_name,
                   LPCSTR window_name,
                   DWORD style,
                   int x,
                   int y,
                   int width,
                   int height,
                   HINSTANCE parent,
                   LPCSTR menu_name,
                   HINSTANCE instance)
{
  cur_instance = instance;
  if (menu_name) {
    window->menu = LoadMenuA (instance, menu_name);
  }
  window->hwnd = CreateWindowExA (ex_style, class_name, window_name, style, x, y, width, height,
                                  parent, window->menu, cur_instance, NULL);
  if (window->hwnd) {
    window->wnd_info = GetWindowLongA (window->hwnd, GWLP_WNDPROC);
    SetWindowLongA (window->hwnd, GWLP_WNDPROC, window->wnd_proc);
    SetPropA (window->hwnd, "ST Object", window);
    window->msg_id = RegisterWindowMessageA ("QueryCancelAutoPlay");
  }
}

/**
 * Deinitialize the TmWindow and free its Win32 resources.
 *
 * @address        0x004A6C98
 *
 * @param[in,out]  window              Window context.
 */
void TmWindowDeinit (TmWindow* window)
{
  if (window->hwnd) {
    ShowWindow (window->hwnd, SW_HIDE);
    if (window->wnd_info && RemovePropA (window->hwnd, "ST Object")) {
      SetWindowLongA (window->hwnd, GWLP_WNDPROC, window->wnd_info);
    }
    if (!GetParent (window->hwnd)) {
      // We're the top level window, so the application is exiting.
      PostQuitMessage (0);
    }
    DestroyWindow (window->hwnd);
    window->hwnd = NULL;
  }
}

/**
 * Retrieve the Win32 window handle for the TmWindow.
 *
 * @address        0x004A6DCC
 *                 0x004A8F39 (thunk)
 *
 * @param[in]      window              Window context.
 * @return         HWND                Handle for the Win32 window.
 */
HWND TmWindowGetHwnd (TmWindow* window)
{
  return window->hwnd;
}

/**
 * Change the window according to the given command value.
 *
 * @address        0x004A6EE4
 *
 * @param[in]      window              Window context.
 * @param[in]      cmd_show            Command value. See Win32 ShowWindow() documentation for
 *                                     more information.
 * @return         0                   The window was previously hidden before this call.
 * @return         other               The window was previously visible before this call.
 */
BOOL TmWindowShow (TmWindow* window, int cmd_show)
{
  return ShowWindow (window->hwnd, cmd_show);
}
