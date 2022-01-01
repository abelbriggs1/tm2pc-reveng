/**
 * @file sound_impl.c
 *
 * Implementation of internal sound functions.
 */

#include "sound_internal.h"

/**
 * Initialize the sound system.
 *
 * @address        0x00486870
 *
 * @param[in,out]  sound              Sound context.
 */
VOID SoundInit (SoundContext* sound)
{
  sound->hwnd = TmWindowGetHwnd ();
  TmRegistryOpenKey (sound->reg_key, HKEY_LOCAL_MACHINE, 0);
  MixerInit (sound);
  TmRegistrySetKeyRaw (sound->reg_key, "UserVolume", &sound->user_vol, 20);
  MixerUpdateVolume (sound, TRUE);
}