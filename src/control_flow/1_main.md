# Main

This file documents the beginning of execution and the entry point `WinMain`.

`WinMain` is located at `0x004A8B04` in the executable and acts as the entry point.
Since TM2PC runs on the Watcom runtime, there's some setup first, but nothing particularly
interesting - `start` sets a function pointer to `&WinMain` and then jumps to a function at
`0x004CC3F2` which sets up the Watcom C standard library and dereferences the function pointer to call `WinMain()`.

The call to `WinMain` is always made with the following arguments:

`WinMain (currentInstance, NULL, cmdLine, SW_SHOWDEFAULT);`

where `currentInstance` is the `HINSTANCE` handle obtained from a call to `GetModuleHandleA(NULL)`
and `cmdLine` is the `LPSTR` command line string obtained from a call to `GetCommandLineA()`.

## Main Skeleton

```c
int WinMain (HINSTANCE instance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd) {

  RECT rc;

  // Set up registry key "Software\Sony Entertainment\Twisted Metal 2\1.0.1"
  PHKEY regkey;

  setRegKeyPath ("Software\Sony Interactive\Twisted Metal 2\1.0.1"); // @ 0x004AC4E8
  createRegKey (&regKey, HKEY_LOCAL_MACHINE, NULL); // @ 0x004AC550
  closeRegKey (&regKey); // @ 0x004AC590
  
  // Process arguments and get Display Capabilities (screen width, height, bits per pixel)
  TMEnvironment env;

  envInit (&env); // @ 0x004C45C0
  
  // Test if the game is running from a CD Drive - save the drive letter in a global variable if so
  findCdDrive ("tm2.ico"); // @ 0x00499884
  
  loadLibrary (&tmLibHandle, "TM2English"); // loadLibrary @ 0x004C4558, tmLibHandle @ 0x00BDFB28
  
  // Open AVI video with the Windows MCI API
  openAviVideo (); // @ 0x004B9BF0
  
  // Initialize Windows COM
  if (CoInitialize (NULL) < 0) {
    return 1;
  }
  
  // Set jump buffer for RtlUnwind in case things go wrong
  BOOL jumpBufSet = setjmp (recoveryMainJumpBuf) == 0; // jmp_buf recoveryMainJumpBuf @ 0x00BDFAF0
  if (jumpBufSet) {
  
    // Detect CDROM, initialize paths to various resources from registry if possible
    // This function returns 1 on success
    int result = initPaths ("tm2.ico"); // @ 0x00498DDC
    if (!result) {
      return !result;
    }
    
    // Calculate main window size and location (default size 640x480)
    int x = 48;
    int y = 48;
    SetRect (&rc, 0, 0, 640, 480);
    AdjustWindowRect (&rc, WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
    openRegKey (&regKey, HKEY_LOCAL_MACHINE, NULL); // @ 0x004AC528
    queryRegKeyDword (&regKey, "Window XPos", &x); // @ 0x004AC5BC
    queryRegKeyDword (&regKey, "Window YPos", &y); // @ 0x004AC5BC
    closeRegKey (&regKey); // @ 0x004AC590
    
    if (rc.right + x > env.screenWidth) {
      x = env.screenWidth - rc.right;
    }
    if (x < 0) {
      x = 0;
    }
    
    if (rc.bottom + y > env.screenHeight) {
      y = env.screenHeight - rc.bottom;
    }
    if (y < 0) {
      y = 0;
    }
    
    // Register window class with name "Twisted Metal 2 Class"
    registerWindowClass ("Twisted Metal 2 Class", instance, CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, NULL); // @ 0x004A6B80
  
    // Initialize window with the expected class name, title bar name ("Twisted Metal 2"), position, and size
    // Save the associated handles in a TMWindow struct
    windowInit ( // @ 0x004A6BE8
       curWindow, // global TMWindow @ 0x00BDFCD0
       WS_EX_APPWINDOW,
       "Twisted Metal 2 Class",
       "Twisted Metal 2",
       WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
       x,
       y,
       rc.right,
       rc.bottom,
       NULL,
       NULL,
       instance
       );
    windowShow (curWindow, nShowCmd); // @ 0x004A6EE4
    
    // Boilerplate Win32 window setup
    RECT rect;
    HWND wnd = windowGetHwnd (curWindow); // @ 0x004A6DCC
    HDC caps = GetDC (wnd);
    GetClientRect (wnd, &rect);
    FillRect (caps, &rect, GetStockObject (BLACK_BRUSH));
    ReleaseDC (wnd, caps);    
  
    // Jump to the main game function
    runGame (envGetNumArgs (&env), envGetCmdArgs (&env), wnd, "Twisted Metal 2"); // @ 0x00477124
  }
  // Various de-init stuff if game function returns
}
```


