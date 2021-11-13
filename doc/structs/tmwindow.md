# TMWindow

This struct stores Win32 handles for the display and associated data.

```c
typedef struct {
  HWND hwnd;
  LONG wndInfo;
  LONG wndProc;
  char pad[4];
  HMENU menu;
  UINT msgId;
} TMWindow;
```
