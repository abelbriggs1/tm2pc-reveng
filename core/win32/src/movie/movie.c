/**
 * @file movie.c
 *
 * Video/movie handling functions.
 */

#include <windows.h>

#include "tm2.h"

#include "internal/win32_internal.h"

#include "movie.h"

static BOOL aviVideoOpen;  // @address 0x00C8511C

/**
 * Initialize movie playback library.
 *
 * @address        0x004B9BF0
 *
 * @return         0                  The library was initialized successfully.
 * @return         others             An error occurred initializing movie playback.
 */
int TmMovieInit ()
{
  MCIERROR result = mciSendStringA ("open avivideo", NULL, NULL, NULL);
  aviVideoOpen = (result == 0);
  return result;
}
