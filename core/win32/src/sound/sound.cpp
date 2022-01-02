/**
 * @file sound.c
 *
 * SoundContext processing functions.
 */

#include "sound_internal.h"

static SoundContext sound;  // @address 0x00624EDC

/**
 * Initialize the sound system.
 *
 * @address        N/A
 */
void TmSoundInit ()
{
  sound.Init ();
}
