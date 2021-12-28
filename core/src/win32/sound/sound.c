/**
 * @file sound.c
 *
 * Sound processing functions.
 */

#include "windows.h"

#include "win32/registry.h"

#include "win32/sound.h"

/**
 * Macros to determine if a Win32 Multimedia System call succeeded
 * or failed.
 */
#define MM_SUCCEEDED(code) ((code == MMSYSERR_NOERROR))
#define MM_FAILED(code) ((code != MMSYSERR_NOERROR))

/**
 * Constants for default volume values (unsigned 16-bit).
 */
#define DEFAULT_VOLUME_CD_UNSIGNED 0x9999    // 60%
#define DEFAULT_VOLUME_WAVE_UNSIGNED 0xE666  // 90%

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
 * Set game volume to default values.
 *
 * @address        0x00486B44
 *
 * @param[in,out]  sound               Sound context.
 */
static void MixerUseDefaultVolume (TmSound* sound)
{
  sound->game_vol[0].waveout_vol = DEFAULT_VOLUME_WAVE_UNSIGNED;
  sound->game_vol[0].cd_vol = DEFAULT_VOLUME_CD_UNSIGNED;
  sound->game_vol[0].bass_vol = 0;
  sound->game_vol[0].treble_vol = 0;

  sound->game_vol[1].waveout_vol = DEFAULT_VOLUME_WAVE_UNSIGNED;
  sound->game_vol[1].cd_vol = DEFAULT_VOLUME_CD_UNSIGNED;
  sound->game_vol[1].bass_vol = 0;
  sound->game_vol[1].treble_vol = 0;

  sound->game_vol[2].waveout_vol = DEFAULT_VOLUME_WAVE_UNSIGNED;
  sound->game_vol[2].cd_vol = DEFAULT_VOLUME_CD_UNSIGNED;
  sound->game_vol[2].bass_vol = 0;
  sound->game_vol[2].treble_vol = 0;
}

/**
 * Get the sound card and initialize volume mixer structures.
 *
 * @address        0x004868CC
 *
 * @param[in,out]  sound               Sound context.
 */
static void MixerInit (TmSound* sound)
{
  MIXERLINEA mixer;
  MIXERCONTROLA ctrl;
  MIXERLINECONTROLSA controls;
  MIXERCONTROLDETAILS details;
  MIXERCONTROLDETAILS_UNSIGNED det;

  if (REG_FAILED (TmRegistryQueryKeyDword (sound->reg_key, "SoundCardID", sound->card))) {
    sound->card = 0;
    TmRegistrySetKeyDword (sound->reg_key, "SoundCardID", sound->card);
  }

  mixer.cbStruct = sizeof (MIXERLINEA);
  mixer.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
  if (MM_SUCCEEDED (mixerGetLineInfoA (sound->card, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE))) {
    controls.dwLineID = mixer.dwLineID;
    controls.cControls = mixer.cControls;
    controls.cbmxctrl = sizeof (MIXERCONTROLA);
    controls.cbStruct = sizeof (MIXERLINECONTROLSA);
    controls.pamxctrl = &ctrl;
    controls.dwControlType = MIXERCONTROL_CONTROLTYPE_BASS;
    if (MM_SUCCEEDED (
          mixerGetLineControlsA (sound->card, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE))) {
      details.dwControlID = ctrl.dwControlID;
      details.cbStruct = sizeof (MIXERCONTROLDETAILS);
      details.cChannels = mixer.cChannels;
      details.cbDetails = sizeof (MIXERCONTROLDETAILS_UNSIGNED);
      details.paDetails = &det;
      details.cMultipleItems = 0;
      mixerGetControlDetailsA (sound->card, &details, MIXER_GETCONTROLDETAILSF_VALUE);
    } else {
      sound->maybe_sound_enabled = 0;
    }
  } else {
    sound->maybe_sound_enabled = 0;
  }

  int bufSize = 60;
  if (REG_FAILED (
        TmRegistryQueryKeyRaw (sound->reg_key, "GameVolume", sound->game_vol, &bufSize))) {
    memcpy (sound->game_vol[0], sound->user_vol, 20);
    memcpy (sound->game_vol[1], sound->user_vol, 20);
    memcpy (sound->game_vol[2], sound->user_vol, 20);
    MixerUseDefaultVolume (sound);
    TmRegistrySetKeyRaw (sound->reg_key, "GameVolume", sound->game_vol, 60);
  }
}

/**
 * Update a Win32 mixer volume with the specified volume level.
 *
 * @address        0x00489340
 *
 * @param[in,out]  sound               Sound context.
 * @param[in]      component_type      Win32 MIXERLINE volume type value.
 * @param[in]      volume              Volume as an unsigned 16-bit integer.
 */
static void MixerUpdateComponentVolume (TmSound* sound, DWORD component_type, DWORD volume)
{
  MIXERLINEA mixer;
  MIXERCONTROLDETAILS details;
  MIXERLINECONTROLS controls;

  mixer.dwComponentType = component_type;
  mixer.cbStruct = sizeof (MIXERLINEA);

  if (MM_FAILED (mixerGetLineInfoA (sound->card, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE))) {
    if (component_type == MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC) {
      int dev = auxGetNumDevs ();
      if (dev > 0) {
        do {
          dev--;
          mixer.cbStruct = sizeof (MIXERLINEA);
          mixerGetLineInfoA (dev, &mixer, MIXER_OBJECTF_AUX);
        } while (!strstr (mixer.szName, "CD") && dev > 0);
      }
    }
  }

  int pamx;
  controls.dwLineID = mixer.dwLineID;
  controls.cControls = mixer.cControls;
  controls.cbmxctrl = sizeof (MIXERCONTROLA);
  controls.pamxctrl = &pamx;
  controls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
  mixerGetLineControlsA (sound->card, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE);

  details.cbStruct = sizeof (MIXERCONTROLDETAILS);
  details.dwControlID = NULL;  // TODO: what value actually gets put here? does it matter?
  details.cChannels = mixer.cChannels;
  details.cbDetails = sizeof (DWORD);
  details.paDetails = &volume;
  details.cMultipleItems = 0;
  return mixerSetControlDetails (sound->card, &details, MIXER_SETCONTROLDETAILSF_VALUE);
}

/**
 * Update all volume levels for the Win32 mixer on the current sound card.
 *
 * @address        0x00486BB4
 *
 * @param[in,out]  sound               Sound context.
 * @param[in]      use_game_vol        TRUE to update with game volume,
 *                                     FALSE to update with user-provided volume.
 */
static void MixerUpdateVolume (TmSound* sound, int use_game_vol)
{
  MMRESULT result;

  MIXERLINEA mixer;
  MIXERCONTROLDETAILS details;
  MIXERLINECONTROLS controls;

  MixerVolume volume;

  if (use_game_vol) {
    // TODO: Indexing into an array with the value of a handle doesn't seem right.
    // What is this code actually doing?
    memcpy (&volume, &sound->game_vol[sound->card], sizeof (MixerVolume));
  } else {
    memcpy (&volume, sound->user_vol, sizeof (MixerVolume));
  }

  MixerUpdateComponentVolume (sound, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, &volume.speaker_vol);
  MixerUpdateComponentVolume (sound, MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC, &volume.cd_vol);
  MixerUpdateComponentVolume (sound, MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, &volume.waveout_vol);

  // Update bass levels
  if (sound->maybe_sound_enabled) {
    mixer.cbStruct = sizeof (MIXERLINEA);
    mixer.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    result = mixerGetLineInfoA (sound->card, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE);
    if (MM_SUCCEEDED (result)) {
      int pamx;
      controls.dwLineID = mixer.dwLineID;
      controls.cbStruct = sizeof (MIXERLINECONTROLSA);
      controls.cControls = mixer.cControls;
      controls.cbmxctrl = sizeof (MIXERCONTROLA);
      controls.pamxctrl = &pamx;
      controls.dwControlType = MIXERCONTROL_CONTROLTYPE_BASS;
      result = mixerGetLineControlsA (sound->card, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE);
      if (MM_SUCCEEDED (result)) {
        details.cbStruct = sizeof (MIXERCONTROLDETAILS);
        details.dwControlID = NULL;  // TODO: what value actually gets put here? does it matter?
        details.cChannels = mixer.cChannels;
        details.cbDetails = sizeof (DWORD);
        details.cMultipleItems = 0;
        details.paDetails = &volume.bass_vol;
        result = mixerSetControlDetails (sound->card, &details, MIXER_SETCONTROLDETAILSF_VALUE);
      } else {
        sound->maybe_sound_enabled = 0;
      }
    } else {
      sound->maybe_sound_enabled = 0;
    }
  }

  // Update treble levels
  if (sound->maybe_sound_enabled) {
    mixer.cbStruct = sizeof (MIXERLINEA);
    mixer.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    result = mixerGetLineInfoA (sound->card, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE);
    if (MM_SUCCEEDED (result)) {
      int pamx;
      controls.dwLineID = mixer.dwLineID;
      controls.cbStruct = sizeof (MIXERLINECONTROLSA);
      controls.cControls = mixer.cControls;
      controls.cbmxctrl = sizeof (MIXERCONTROLA);
      controls.pamxctrl = &pamx;
      controls.dwControlType = MIXERCONTROL_CONTROLTYPE_TREBLE;
      result = mixerGetLineControlsA (sound->card, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE);
      if (MM_SUCCEEDED (result)) {
        details.cbStruct = sizeof (MIXERCONTROLDETAILS);
        details.dwControlID = NULL;  // TODO: what value actually gets put here? does it matter?
        details.cChannels = mixer.cChannels;
        details.cbDetails = sizeof (DWORD);
        details.cMultipleItems = 0;
        details.paDetails = &volume.treble_vol;
        result = mixerSetControlDetails (sound->card, &details, MIXER_SETCONTROLDETAILSF_VALUE);
      } else {
        sound->maybe_sound_enabled = 0;
      }
    } else {
      sound->maybe_sound_enabled = 0;
    }
  }
}

/**
 * Initialize the sound system.
 *
 * @address        0x00486870
 *
 * @param[in,out]  sound              Sound context.
 */
void TmSoundInit (TmSound* sound)
{
  sound->hwnd = TmWindowGetHwnd ();  // thunk 0x004A8F34
  TmRegistryOpenKey (sound->reg_key, HKEY_LOCAL_MACHINE, 0);
  MixerInit (sound);
  TmRegistrySetKeyRaw (sound->reg_key, "UserVolume", sound->user_vol, 20);
  MixerUpdateVolume (sound, TRUE);
}
