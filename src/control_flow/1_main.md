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

## Main Pseudocode Skeleton

```c
int WinMain (HINSTANCE instance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd) {

  // Set up registry keys "Software\SingleTrac\default" and "Software\Sony Entertainment\Twisted Metal 2\1.0.1"
  
  // Save command line string and get Display Capabilities (screen width, height, bits per pixel) in a huge struct
  
  // Test if the game is running from a CD Drive - save the drive letter in a global variable if so
  
  // Open AVI video with the Windows MCI API (command string: "open avivideo")
  
  // Initialize Windows COM via CoInitialize(NULL)
  //   if error, return 1
  
  // Detect installation directory (are we running on CD or locally?)
  //   if we couldn't detect, return error
  
  // Calculate main window size and location (default size 640x480)
  
  // Register window class with name "Twisted Metal 2 Class"
  
  // Initialize window with the expected class name, title bar name ("Twisted Metal 2"), position, and size
  //   save the associated handles in a TMWindow struct
  
  // Jump to the main game function @ 0x00477124
  
  // Various de-init stuff if game function returns
}
```
  
  
  
