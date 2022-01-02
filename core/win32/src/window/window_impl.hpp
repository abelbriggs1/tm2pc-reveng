/**
 * @file window_impl.h
 *
 * Implementation of internal Win32 window functions.
 */

#ifndef TM_WINDOW_IMPL_H
#define TM_WINDOW_IMPL_H

class WindowContext {
 public:
  VOID Init (DWORD ex_style,
             LPCSTR class_name,
             LPCSTR window_name,
             DWORD style,
             DWORD x,
             DWORD y,
             DWORD width,
             DWORD height,
             HWND parent,
             LPCSTR menu_name,
             HINSTANCE instance);
  VOID Deinit ();
  HWND GetHwnd ();
  BOOL Show (int cmd_show);

 private:
  HWND hwnd;
  LONG wnd_info;
  LONG wnd_proc;
  CHAR pad[4];  // unknown if padding or variable
  HMENU menu;
  UINT msg_id;
};

#endif /* TM_WINDOW_IMPL_H */