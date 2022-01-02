/**
 * @file window_impl.c
 *
 * Implementation of internal Win32 window functions.
 */

#include "window_internal.h"

static HINSTANCE cur_instance;  // @address 0x00BDFAE8

/**
 * Initialize a Window with the specified parameters.
 *
 * @address        0x004A6BE8
 *
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
VOID WindowContext::Init (DWORD ex_style,
                          LPCSTR class_name,
                          LPCSTR window_name,
                          DWORD style,
                          DWORD x,
                          DWORD y,
                          DWORD width,
                          DWORD height,
                          HWND parent,
                          LPCSTR menu_name,
                          HINSTANCE instance)
{
  cur_instance = instance;
  if (menu_name) {
    this->menu = LoadMenuA (instance, menu_name);
  }
  this->hwnd = CreateWindowExA (ex_style, class_name, window_name, style, x, y, width, height,
                                parent, this->menu, cur_instance, NULL);
  if (this->hwnd) {
    this->wnd_info = GetWindowLongA (this->hwnd, GWLP_WNDPROC);
    SetWindowLongA (this->hwnd, GWLP_WNDPROC, this->wnd_proc);
    SetPropA (this->hwnd, "ST Object", (HANDLE)this);
    this->msg_id = RegisterWindowMessageA ("QueryCancelAutoPlay");
  }
}

/**
 * Deinitialize the Window and free its Win32 resources.
 *
 * @address        0x004A6C98
 */
VOID WindowContext::Deinit ()
{
  if (this->hwnd) {
    ShowWindow (this->hwnd, SW_HIDE);
    if (this->wnd_info && RemovePropA (this->hwnd, "ST Object")) {
      SetWindowLongA (this->hwnd, GWLP_WNDPROC, this->wnd_info);
    }
    if (!GetParent (this->hwnd)) {
      // We're the top level window, so the application is exiting.
      PostQuitMessage (0);
    }
    DestroyWindow (this->hwnd);
    this->hwnd = NULL;
  }
}

/**
 * Retrieve the Win32 window handle for the Window.
 *
 * @address        0x004A6DCC
 *
 * @return         HWND                Handle for the Win32 window.
 */
HWND WindowContext::GetHwnd ()
{
  return this->hwnd;
}

/**
 * Change the window according to the given command value.
 *
 * @address        0x004A6EE4
 *
 * @param[in]      cmd_show            Command value. See Win32 ShowWindow() documentation for
 *                                     more information.
 *
 * @return         0                   The window was previously hidden before this call.
 * @return         other               The window was previously visible before this call.
 */
BOOL WindowContext::Show (int cmd_show)
{
  return ShowWindow (this->hwnd, cmd_show);
}