/**
 * @file sound_impl.h
 *
 * Implementation of internal sound functions.
 */

#ifndef TM_SOUND_IMPL_H
#define TM_SOUND_IMPL_H

/**
 * Macros to determine if a Win32 Multimedia System call succeeded
 * or failed.
 */
#define MM_SUCCEEDED(code) ((code == MMSYSERR_NOERROR))
#define MM_FAILED(code) ((code != MMSYSERR_NOERROR))

/**
 * Internal structure to keep track of mixer volume levels.
 */
typedef struct {
  DWORD speaker_vol;
  DWORD waveout_vol;
  DWORD cd_vol;
  DWORD bass_vol;
  DWORD treble_vol;
} MixerVolume;

class SoundContext {
 public:
  VOID Init ();

 private:
  HWND hwnd;
  CHAR unk1[48];  // unknown
  MixerVolume user_vol;
  MixerVolume game_vol[3];   // TODO: not sure why there are multiple of these
  CHAR unk2[4252];           // unknown
  BOOL maybe_sound_enabled;  // TODO: this appears to be the global sound enable/disable flag
  CHAR unk3[4];              // unknown
  HMIXEROBJ card;
  PHKEY reg_key;

  VOID MixerInit ();
  VOID MixerUpdateVolume (DWORD use_game_vol);
  VOID MixerUseDefaultVolume ();
  MMRESULT MixerUpdateComponentVolume (DWORD component_type, DWORD volume);
};

#endif /* TM_SOUND_IMPL_H */