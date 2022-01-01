#ifndef TM_MIXER_H
#define TM_MIXER_H

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

/**
 * Get the sound card and initialize volume mixer structures.
 *
 * @param[in,out]  sound               Sound context.
 */
VOID MixerInit (SoundContext* sound);

/**
 * Update all volume levels for the Win32 mixer on the current sound card.
 *
 * @param[in,out]  sound               Sound context.
 * @param[in]      use_game_vol        TRUE to update with game volume,
 *                                     FALSE to update with user-provided volume.
 */
VOID MixerUpdateVolume (SoundContext* sound, DWORD use_game_vol);

#endif /* TM_MIXER_H */