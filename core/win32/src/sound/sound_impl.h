/**
 * @file sound_impl.h
 *
 * Implementation of internal sound functions.
 */

#ifndef TM_SOUND_IMPL_H
#define TM_SOUND_IMPL_H

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
} SoundContext;  // total size 0x1130 bytes

/**
 * Initialize the internal sound system.
 *
 * @param[in,out]  sound              Sound context.
 */
VOID SoundInit (SoundContext* sound);

#endif /* TM_SOUND_IMPL_H */