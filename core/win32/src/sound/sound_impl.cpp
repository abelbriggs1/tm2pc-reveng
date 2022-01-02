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
 */
VOID SoundContext::Init ()
{
  this->hwnd = TmWindowGetHwnd ();
  TmRegistryOpenKey (this->reg_key, HKEY_LOCAL_MACHINE, 0);
  MixerInit ();
  TmRegistrySetKeyRaw (this->reg_key, "UserVolume", (LPCBYTE) & this->user_vol,
                       sizeof (this->user_vol));
  MixerUpdateVolume (TRUE);
}