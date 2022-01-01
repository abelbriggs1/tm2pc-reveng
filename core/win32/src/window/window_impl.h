/**
 * @file window_impl.h
 *
 * Implementation of internal Win32 window functions.
 */

#ifndef TM_WINDOW_IMPL_H
#define TM_WINDOW_IMPL_H

/**
 * Structure for Win32 data for the current window.
 *
 * TODO: The window functions all take a WindowContext*, so this may be a C++ class.
 * Investigate further and refactor accordingly.
 */
typedef struct {
  HWND hwnd;
  LONG wnd_info;
  LONG wnd_proc;
  CHAR pad[4];  // unknown if padding or variable
  HMENU menu;
  UINT msg_id;
} WindowContext;

/**
 * Initialize a Window with the specified parameters.
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
VOID WindowInit (WindowContext* window,
                 DWORD ex_style,
                 LPCSTR class_name,
                 LPCSTR window_name,
                 DWORD style,
                 DWORD x,
                 DWORD y,
                 DWORD width,
                 DWORD height,
                 HINSTANCE parent,
                 LPCSTR menu_name,
                 HINSTANCE instance);

/**
 * Deinitialize the Window and free its Win32 resources.
 *
 * @param[in,out]  window              Window context.
 */
VOID WindowDeinit (WindowContext* window);

/**
 * Retrieve the Win32 window handle for the Window.
 *
 * @param[in]      window              Window context.
 * @return         HWND                Handle for the Win32 window.
 */
HWND WindowGetHwnd (WindowContext* window);

/**
 * Change the window according to the given command value.
 *
 * @param[in]      window              Window context.
 * @param[in]      cmd_show            Command value. See Win32 ShowWindow() documentation for
 *                                     more information.
 * @return         0                   The window was previously hidden before this call.
 * @return         other               The window was previously visible before this call.
 */
BOOL WindowShow (WindowContext* window, int cmd_show);

#endif /* TM_WINDOW_IMPL_H */