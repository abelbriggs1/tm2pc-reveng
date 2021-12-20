/**
 * @file video.h
 *
 * Win32 video/movie handling functions.
 */

#ifndef TM_MOVIE_H
#define TM_MOVIE_H

/**
 * Initialize Win32 MCI movie playback library.
 *
 * @return         MCIERROR            Status code from Windows MCI.
 */
MCIERROR TmMovieInit ();

#endif /* TM_MOVIE_H */
