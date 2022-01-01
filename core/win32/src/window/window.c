/**
 * @file window.c
 *
 * Win32 window handling functions.
 */

#include "window_internal.h"

static WindowContext window;  // @address 0x00BDFCD0

/**
 * Initialize the application's Win32 window with the specified parameters.
 *
 * @address        N/A
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
VOID TmWindowInit (DWORD ex_style,
                   LPCSTR class_name,
                   LPCSTR window_name,
                   DWORD style,
                   DWORD x,
                   DWORD y,
                   DWORD width,
                   DWORD height,
                   HINSTANCE parent,
                   LPCSTR menu_name,
                   HINSTANCE instance)
{
  WindowInit (&window, ex_style, class_name, window_name, style, x, y, width, height, parent,
              menu_name, instance);
}

/**
 * Deinitialize the application's Win32 window and free its resources.
 *
 * @address        N/A
 */
VOID TmWindowDeinit ()
{
  WindowDeinit (&window);
}

/**
 * Retrieve the Win32 handle for the application's window.
 *
 * @address        0x004A8F34
 *
 * @return         HWND                Handle for the Win32 window.
 */
HWND TmWindowGetHwnd ()
{
  return WindowGetHwnd (&window);
}

/**
 * Change the window according to the given command value.
 *
 * @address        N/A
 *
 * @param[in]      cmd_show            Command value. See Win32 ShowWindow() documentation for
 *                                     more information.
 * @return         0                   The window was previously hidden before this call.
 * @return         other               The window was previously visible before this call.
 */
BOOL TmWindowShow (int cmd_show)
{
  return WindowShow (&window, cmd_show);
}