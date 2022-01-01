/**
 * @file error.c
 *
 * Win32-specific error handling.
 */

#include <windows.h>

#include <stdarg.h>

#include "tm2.h"

#include "internal/win32_internal.h"

#define RETRY_YES (MB_ICONEXCLAMATION | MB_RETRYCANCEL)  // 0x35
#define RETRY_NO (MB_ICONEXCLAMATION | MB_OK)            // 0x30

/**
 * Represents user-visible data for a Win32 message box.
 */
typedef struct {
  UINT type;
  LPCSTR caption;
  LPCSTR text;
} TmErrorMsgBox;

/**
 * Table of message box parameters and strings for Win32 message boxes. This can be
 * indexed with TmErrorCode values.
 *
 * @address        0x004E8840
 */
static const TmErrorMsgBox error_table[TM_ERROR_MAX] = {
  {
    RETRY_YES,
    "CD Audio Error",
    "Another program has the CDAUDIO device locked.\n\n\
     Please quit any other running CDROM programs. \
     Click RETRY to retry or click CANCEL to play without music.",
  },
  {
    RETRY_YES,
    "CD Audio Error",
    "There is a problem with your CDAUDIO device.\n\n\
     Make sure the CD is in the drive. Click RETRY to retry or click CANCEL to continue.",
  },
  {
    RETRY_NO,
    "Movie Error",
    "An error occurred while trying to get a handle to the movie window.\n\n\
     This movie will not be played.",
  },
  {
    RETRY_NO,
    "Movie Error",
    "An error occurred while trying to open the movie '%s'.\n\n\
     This movie will not be played.",
  },
  {
    RETRY_NO,
    "ActiveMovie Error",
    "An error occurred while trying to initialize ActiveMovie.",
  },
  {
    RETRY_NO,
    "ActiveMovie Error",
    "An error occurred while trying to render the movie '%s'.\n\n\
     This movie will not be played.",
  },
  {
    RETRY_NO,
    "ActiveMovie Error",
    "An error occurred while trying to play the movie.",
  },
  {
    RETRY_NO,
    "ActiveMovie Error",
    "An error occurred while trying to pause the movie.",
  },
  {
    RETRY_NO,
    "ActiveMovie Error",
    "An error occurred while trying to stop the movie.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "A DirectDraw object could not be created (code: %i).\n\n\
     You may need to restart your computer and/or reinstall DirectX.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "The program tried changing to an unsupported video mode (mode: %i x %i in %i bit color).\n\n\
     Please make sure your that your computer supports the minimum requirements.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "The program could not get a pointer to the back buffer (code: %i).\n\n\
     You may need to restart your computer and/or reinstall DirectX.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "The program could not create a primary surface (code: %i).\n\n\
     You may need to restart your computer and/or reinstall DirectX.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "The program could not create any back buffers (code: %i).\n\n\
     You may need to restart your computer and/or reinstall DirectX.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "The program could not set the cooperative level (code: %i).\n\n\
     Quit any other DirectDraw programs and try running this game again.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "The program could not create a system memory buffer (code: %i).\n\n\
     Try quitting all other running programs and rerun this game.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "The program could not create a clipper object (code: %i).\n\n\
     Program execution will continue.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "The program could not associate the clipper object with the GDI (code: %i).\n\n\
     Program execution will continue.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "The program could not attach the clipper object to the primary surface (code: %i).\n\n\
     Program execution will continue.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "DirectDraw does not appear to be supported.\n\n\
     If you are in 16 (4-bit) color mode, please change to at least 256 (8-bit) color mode. \
     Program execution will terminate.",
  },
  {
    RETRY_NO,
    "DirectDraw Error",
    "There is no DirectDraw hardware support.\n\n\
     This may have been caused by changing the video card and not reinstalling DirectX \
     or you may not have a video card installed that supports DirectDraw.\n\
     Try reinstalling the game or getting a new DirectDraw video driver from your video card manufacturer. \
     Program execution will terminate.",
  },
  {
    RETRY_NO,
    "DirectSound Error",
    "DirectSound could not be initialized.\n\n\
     The game will continue without sound effects.",
  },
  {
    RETRY_YES,
    "CDROM Error",
    "The game disk is not in the CDROM drive.\n\n\
     Please insert the CDROM and click RETRY to retry or click CANCEL to quit.",
  },
  {
    RETRY_NO,
    "DirectPlay Error",
    "Could not open a DirectPlay game session.\n\
     Error Number : %d\n\
     Error Description : %s.",
  },
  {
    RETRY_NO,
    "DirectPlay Error",
    "Could not open a DirectPlay game session.\n\
     Error Number : %d\n\
     Error Description : %s.",
  },
  {
    RETRY_NO,
    "DirectPlay Error",
    "There was an error trying to enumerate players.\n\
     Error Number : %d\n\
     Error Description : %s.",
  },
  {
    RETRY_NO,
    "DirectPlay Error",
    "There was an error trying to enumerate sessions.\n\
     Error Number : %d\n\
     Error Description : %s.",
  },
  {
    RETRY_NO,
    "DirectPlay Error",
    "Could not creat a DirectPlay object with current provider.\n\
     Error Number : %d\n\
     Error Description : %s\n\
     Provider : %s.",
  },
  {
    RETRY_NO,
    "Network Error",
    "Network data search encountered an invalid link.\n\
     Error Number : %d",
  },
  {
    RETRY_NO,
    "Network Error",
    "Network data requested is out of range.\n\
     Error Number : %d",
  },
  {
    RETRY_NO,
    "Network Error",
    "Network could not resolve a player ID.\n\
     Player Number : %d",
  },
  {
    RETRY_NO,
    "Network Error",
    "The Network is not available.\n\
     Error Number : %d\n\
     Error Description : %s",
  },
};

/**
 * Displays an error message to the user for the given error code and retrieves
 * the user's response.
 *
 * @address        0x004BC660
 *
 * @param[in]      code            Error code for the error.
 * @param[in]      ...             Variadic arguments for various error strings.

 * @return         int             Menu item value selected by the user in the Win32 message box.
 */
int TmErrorMessage (TmErrorCode code, ...)
{
  char text[812];

  va_list arglist;
  va_start (arglist, code);

  wvsprintfA (text, error_table[code].text, arglist);
  ShowCursor (TRUE);
  HWND curHwnd = TmWindowGetHwnd ();  // calls thunked version
  HGDIOBJ obj = GetStockObject (BLACK_BRUSH);
  DWORD prevLong = SetClassLongA (curHwnd, GCLP_HBRBACKGROUND, obj);
  int result = MessageBoxA (curHwnd, text, error_table[code].caption, error_table[code].type);
  ShowCursor (FALSE);
  SetClassLongA (curHwnd, GCLP_HBRBACKGROUND, prevLong);
  va_end (arglist);
  return result;
}
