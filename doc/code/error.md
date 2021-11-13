# Error Handling

```c
#define TM_ERROR_RETRY_YES (MB_ICONEXCLAMATION | MB_RETRYCANCEL) // 0x35
#define TM_ERROR_RETRY_NO  (MB_ICONEXCLAMATION | MB_OK) // 0x30

static TMError tmErrorTable[32] { @ 0x004E8840
  {
    TM_ERROR_RETRY_YES,
    "CD Audio Error",
    "Another program has the CDAUDIO device locked.\n\n\
     Please quit any other running CDROM programs. \
     Click RETRY to retry or click CANCEL to play without music."
  },
  {
    TM_ERROR_RETRY_YES,
    "CD Audio Error",
    "There is a problem with your CDAUDIO device.\n\n\
     Make sure the CD is in the drive. Click RETRY to retry or click CANCEL to continue."
  },
  {
    TM_ERROR_RETRY_NO,
    "Movie Error",
    "An error occurred while trying to get a handle to the movie window.\n\n\
     This movie will not be played."
  },
  {
    TM_ERROR_RETRY_NO,
    "Movie Error",
    "An error occurred while trying to open the movie '%s'.\n\n\
     This movie will not be played."
  },
  {
    TM_ERROR_RETRY_NO,
    "ActiveMovie Error",
    "An error occurred while trying to initialize ActiveMovie."
  },
  {
    TM_ERROR_RETRY_NO,
    "ActiveMovie Error",
    "An error occurred while trying to render the movie '%s'.\n\n\
     This movie will not be played."
  },
  {
    TM_ERROR_RETRY_NO,
    "ActiveMovie Error",
    "An error occurred while trying to play the movie."
  },
  {
    TM_ERROR_RETRY_NO,
    "ActiveMovie Error",
    "An error occurred while trying to pause the movie."
  },
  {
    TM_ERROR_RETRY_NO,
    "ActiveMovie Error",
    "An error occurred while trying to stop the movie."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "A DirectDraw object could not be created (code: %i).\n\n\
     You may need to restart your computer and/or reinstall DirectX."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "The program tried changing to an unsupported video mode (mode: %i x %i in %i bit color).\n\n\
     Please make sure your that your computer supports the minimum requirements."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "The program could not get a pointer to the back buffer (code: %i).\n\n\
     You may need to restart your computer and/or reinstall DirectX."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "The program could not create a primary surface (code: %i).\n\n\
     You may need to restart your computer and/or reinstall DirectX."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "The program could not create any back buffers (code: %i).\n\n\
     You may need to restart your computer and/or reinstall DirectX."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "The program could not set the cooperative level (code: %i).\n\n\
     Quit any other DirectDraw programs and try running this game again."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "The program could not create a system memory buffer (code: %i).\n\n\
     Try quitting all other running programs and rerun this game."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "The program could not create a clipper object (code: %i).\n\n\
     Program execution will continue."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "The program could not associate the clipper object with the GDI (code: %i).\n\n\
     Program execution will continue."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "The program could not attach the clipper object to the primary surface (code: %i).\n\n\
     Program execution will continue."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "DirectDraw does not appear to be supported.\n\n\
     If you are in 16 (4-bit) color mode, please change to at least 256 (8-bit) color mode. \
     Program execution will terminate."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectDraw Error",
    "There is no DirectDraw hardware support.\n\n\
     This may have been caused by changing the video card and not reinstalling DirectX \
     or you may not have a video card installed that supports DirectDraw.\n\
     Try reinstalling the game or getting a new DirectDraw video driver from your video card manufacturer. \
     Program execution will terminate."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectSound Error",
    "DirectSound could not be initialized.\n\n\
     The game will continue without sound effects."
  },
  {
    TM_ERROR_RETRY_YES,
    "CDROM Error",
    "The game disk is not in the CDROM drive.\n\n\
     Please insert the CDROM and click RETRY to retry or click CANCEL to quit."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectPlay Error",
    "Could not open a DirectPlay game session.\n\
     Error Number : %d\n\
     Error Description : %s."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectPlay Error",
    "Could not open a DirectPlay game session.\n\
     Error Number : %d\n\
     Error Description : %s."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectPlay Error",
    "There was an error trying to enumerate players.\n\
     Error Number : %d\n\
     Error Description : %s."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectPlay Error",
    "There was an error trying to enumerate sessions.\n\
     Error Number : %d\n\
     Error Description : %s."
  },
  {
    TM_ERROR_RETRY_NO,
    "DirectPlay Error",
    "Could not creat a DirectPlay object with current provider.\n\
     Error Number : %d\n\
     Error Description : %s\n\
     Provider : %s."
  },
  {
    TM_ERROR_RETRY_NO,
    "Network Error",
    "Network data search encountered an invalid link.\n\
     Error Number : %d"
  },
  {
    TM_ERROR_RETRY_NO,
    "Network Error",
    "Network data requested is out of range.\n\
     Error Number : %d"
  },
  {
    TM_ERROR_RETRY_NO,
    "Network Error",
    "Network could not resolve a player ID.\n\
     Player Number : %d"
  },
  {
    TM_ERROR_RETRY_NO,
    "Network Error",
    "The Network is not available.\n\
     Error Number : %d\n\
     Error Description : %s"
  }
};
```

```c
int DisplayErrorMessage (ERROR_CODE code, ...) { // @ 0x004BC660
  char text[812];

  VA_LIST arglist;
  VA_START (arglist, code);
  
  wvsprintfA (text, tmErrorTable[code].errorStrDescription, arglist);
  ShowCursor (TRUE);
  HWND curHwnd = windowGetHwndThunk (); // @ 0x004A8F39, thunk for windowGetHwnd @ 0x004A6DCC
  HGDIOBJ obj = GetStockObject (BLACK_BRUSH);
  DWORD prevLong = SetClassLongA (curHwnd, GCL_HBRBACKGROUND, obj);
  int result = MessageBoxA (
                 curHwnd,
                 text,
                 tmErrorTable[code].errorStrTitle,
                 tmErrorTable[code].msgBoxType
                 );
  ShowCursor (FALSE);
  SetClassLongA (curHwnd, GCL_HBRBACKGROUND, prevLong);
  return result;
}
```
