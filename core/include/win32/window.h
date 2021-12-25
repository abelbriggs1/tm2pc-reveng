/**
 * @file window.h
 *
 * Win32 window handling functions.
 */

#ifndef TM_WINDOW_H
#define TM_WINDOW_H

/**
 * Structure for Win32 data for the current window.
 *
 * TODO: The window functions all take a TmWindow*, so this may be a C++ class.
 * Investigate further and refactor accordingly.
 */
typedef struct {
  HWND hwnd;
  LONG wnd_info;
  LONG wnd_proc;
  char pad[4];  // unknown if padding or variable
  HMENU menu;
  UINT msg_id;
} TmWindow;

/**
 * Initialize a TmWindow with the specified parameters.
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
                   HINSTANCE instance);

/**
 * Deinitialize the TmWindow and free its Win32 resources.
 *
 * @param[in,out]  window              Window context.
 */
void TmWindowDeinit (TmWindow* window);

/**
 * Retrieve the Win32 window handle for the TmWindow.
 *
 * @param[in]      window              Window context.
 * @return         HWND                Handle for the Win32 window.
 */
HWND TmWindowGetHwnd (TmWindow* window);

/**
 * Change the window according to the given command value.
 *
 * @param[in]      window              Window context.
 * @param[in]      cmd_show            Command value. See Win32 ShowWindow() documentation for
 *                                     more information.
 * @return         0                   The window was previously hidden before this call.
 * @return         other               The window was previously visible before this call.
 */
BOOL TmWindowShow (TmWindow* window, int cmd_show);

#endif /* TM_WINDOW_H */