# Window Functions

`TMWindow curWindow; // global @ 0x00BDFCD0`
`HINSTANCE curInstance; // @ 0x00BDFAE8`

```c
void windowInit ( // @ 0x004A6BE8
       TMWindow * window,
       DWORD exStyle,
       const char * className,
       const char * windowName,
       int style,
       int x,
       int y,
       int width,
       int height,
       HINSTANCE parent,
       LPCSTR menuName,
       HINSTANCE instance
       ) {
  curInstance = instance;
  if (menuName) {
    window->menu = LoadMenuA (instance, menuName);
  }
  window->hwnd = CreateWindowExA (
                    exStyle,
                    className,
                    windowName,
                    style,
                    X,
                    Y,
                    width,
                    height,
                    parent,
                    window->menu,
                    curInstance,
                    NULL
                    );
  if (window->hwnd) {
    window->wndInfo = GetWindowLong (window->hwnd, GWL_WNDPROC);
    SetWindowLongA (window->hwnd, GWL_WNDPROC, window->wndProc);
    SetPropA (window->hwnd, "ST Object", window);
    window->msgId = RegisterWindowMessageA ("QueryCancelAutoPlay");
  }
}               
```

```c
HWND windowGetHwnd (TMWindow * window) { // @ 0x004A6DCC
  return window->hwnd;
}
```

```c
BOOL windowShow (TMWindow * window, int cmdShow) { // @ 0x004A6EE4
  return ShowWindow (window->hwnd, cmdShow);
}
```

```c
// should probably be documented in a different file, this is only called by WinMain
// doesn't deal with TMWindow at all
ATOM registerWindowClass (const char * className, HINSTANCE instance, UINT style, HBRUSH bg) { // @ 0x004A6B80
  WNDCLASSA wClass;
  
  wClass.style = style;
  wClass.instance = instance;
  wClass.lpfnWndProc = DefWindowProcA;
  wClass.cbClsExtra = NULL;
  wClass.cbWndExtra = NULL;
  wClass.hIcon = LoadIconA (instance, "GAME_ICON");
  wClass.hCursor = LoadCursorA (NULL, IDC_ARROW);
  wClass.hbrBackground = bg;
  wClass.lpszMenuName = NULL;
  wClass.lpszClassName = className;
  return RegisterClassA (&wClass);
}
```