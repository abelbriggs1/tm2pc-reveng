/**
 * @file video.c
 *
 * Win32 video/movie handling functions.
 */

#include <windows.h>

#include "movie.h"

static BOOL aviVideoOpen;  // @address 0x00C8511C

/**
 * Initialize Win32 MCI movie playback library.
 *
 * @address        0x004B9BF0
 *
 * @return         MCIERROR            Status code from Windows MCI.
 */
MCIERROR TmMovieInit ()
{
  MCIERROR result = mciSendStringA ("open avivideo", NULL, NULL, NULL);
  aviVideoOpen = (result == 0);
  return result;
}
