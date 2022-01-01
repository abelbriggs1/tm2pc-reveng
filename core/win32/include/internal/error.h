/**
 * @file error.h
 *
 * Win32-specific error handling.
 */

#ifndef TM_ERROR_H
#define TM_ERROR_H

/**
 * Error codes for various errors which can be passed to ErrorMessage().
 */
typedef enum {
  TM_ERROR_CD_DRIVE_LOCKED = 0,
  TM_ERROR_CD_DRIVE_ERROR = 1,
  TM_ERROR_MOVIE_NO_HANDLE = 2,
  TM_ERROR_MOVIE_COULD_NOT_OPEN = 3,
  TM_ERROR_MOVIE_ACTIVEMOVIE_INIT_FAIL = 4,
  TM_ERROR_MOVIE_ACTIVEMOVIE_RENDER_FAIL = 5,
  TM_ERROR_MOVIE_ACTIVEMOVIE_PLAY_FAIL = 6,
  TM_ERROR_MOVIE_ACTIVEMOVIE_PAUSE_FAIL = 7,
  TM_ERROR_MOVIE_ACTIVEMOVIE_STOP_FAIL = 8,
  TM_ERROR_DRAW_CREATE_OBJ_FAIL = 9,
  TM_ERROR_DRAW_UNSUPPORTED_VIDEO_MODE = 10,
  TM_ERROR_DRAW_NO_BACKBUFFER = 11,
  TM_ERROR_DRAW_SURFACE_FAIL = 12,
  TM_ERROR_DRAW_CREATE_BACKBUFFER_FAIL = 13,
  TM_ERROR_DRAW_COOP_LEVEL_FAIL = 14,
  TM_ERROR_DRAW_CREATE_MEMBUFFER_FAIL = 15,
  TM_ERROR_DRAW_CREATE_CLIPPER_FAIL = 16,
  TM_ERROR_DRAW_ASSOC_CLIPPER_FAIL = 17,
  TM_ERROR_DRAW_ATTACH_CLIPPER_FAIL = 18,
  TM_ERROR_DRAW_UNSUPPORTED = 19,
  TM_ERROR_DRAW_NO_HARDWARE = 20,
  TM_ERROR_SOUND_INIT_FAIL = 21,
  TM_ERROR_CD_INSERT_CD = 22,
  TM_ERROR_PLAY_INIT_FAIL = 23,
  TM_ERROR_PLAY_INIT_FAIL_2 = 24,
  TM_ERROR_PLAY_ENUM_PLAYERS_FAIL = 25,
  TM_ERROR_PLAY_ENUM_SESSION_FAIL = 26,
  TM_ERROR_PLAY_CREATE_OBJ_FAIL = 27,
  TM_ERROR_NET_INVALID_LINK = 28,
  TM_ERROR_NET_OUT_OF_RANGE = 29,
  TM_ERROR_NET_PLAYER_ID_FAIL = 30,
  TM_ERROR_NET_NOT_AVAILABLE = 31,
  TM_ERROR_MAX
} TmErrorCode;

/**
 * Displays an error message to the user for the given error code and retrieves
 * the user's response.
 *
 * @param[in]      code            Error code for the error.
 * @param[in]      ...             Variadic arguments for various error strings.

 * @return         int             Menu item value selected by the user in the Win32 message box.
 */
int TmErrorMessage (TmErrorCode code, ...);

#endif /* TM_ERROR_H */