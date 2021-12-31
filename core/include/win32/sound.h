/**
 * @file sound.h
 *
 * Sound structures and functions.
 */

#ifndef TM_SOUND_H
#define TM_SOUND_H

typedef struct {
  HWND hwnd;
  CHAR unk1[48];  // unknown
  MixerVolume user_vol;
  MixerVolume game_vol[3];   // TODO: not sure why there are multiple of these
  CHAR unk2[4252];           // unknown
  BOOL maybe_sound_enabled;  // TODO: this appears to be the global sound enable/disable flag
  CHAR unk3[4];              // unknown
  HMIXEROBJ card;
  PHKEY reg_key;
} TmSound;  // total size 0x1130 bytes

/**
 * Initialize the sound system.
 *
 * @param[in,out]  sound              Sound context.
 */
VOID TmSoundInit (TmSound* sound);

#endif /* TM_SOUND_H */