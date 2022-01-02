/**
 * @file mixer.c
 *
 * Sound mixer functions.
 */

#include "sound_internal.h"

/**
 * Constants for default volume values (unsigned 16-bit).
 */
#define DEFAULT_VOLUME_CD_UNSIGNED 0x9999    // 60%
#define DEFAULT_VOLUME_WAVE_UNSIGNED 0xE666  // 90%

/**
 * Set game volume to default values.
 *
 * @address        0x00486B44
 */
VOID SoundContext::MixerUseDefaultVolume ()
{
  this->game_vol[0].waveout_vol = DEFAULT_VOLUME_WAVE_UNSIGNED;
  this->game_vol[0].cd_vol = DEFAULT_VOLUME_CD_UNSIGNED;
  this->game_vol[0].bass_vol = 0;
  this->game_vol[0].treble_vol = 0;

  this->game_vol[1].waveout_vol = DEFAULT_VOLUME_WAVE_UNSIGNED;
  this->game_vol[1].cd_vol = DEFAULT_VOLUME_CD_UNSIGNED;
  this->game_vol[1].bass_vol = 0;
  this->game_vol[1].treble_vol = 0;

  this->game_vol[2].waveout_vol = DEFAULT_VOLUME_WAVE_UNSIGNED;
  this->game_vol[2].cd_vol = DEFAULT_VOLUME_CD_UNSIGNED;
  this->game_vol[2].bass_vol = 0;
  this->game_vol[2].treble_vol = 0;
}

/**
 * Update a Win32 mixer volume with the specified volume level.
 *
 * @address        0x00489340
 *
 * @param[in]      component_type      Win32 MIXERLINE volume type value.
 * @param[in]      volume              Volume as an unsigned 16-bit integer.
 */
MMRESULT SoundContext::MixerUpdateComponentVolume (DWORD component_type, DWORD volume)
{
  MIXERLINEA mixer;
  MIXERCONTROLDETAILS details;
  MIXERLINECONTROLS controls;

  mixer.dwComponentType = component_type;
  mixer.cbStruct = sizeof (MIXERLINEA);

  if (MM_FAILED (mixerGetLineInfoA (this->card, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE))) {
    if (component_type == MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC) {
      int dev = auxGetNumDevs ();
      if (dev > 0) {
        do {
          dev--;
          mixer.cbStruct = sizeof (MIXERLINEA);
          mixerGetLineInfoA ((HMIXEROBJ)dev, &mixer, MIXER_OBJECTF_AUX);
        } while (!strstr (mixer.szName, "CD") && dev > 0);
      }
    }
  }

  MIXERCONTROLA pamx;
  controls.dwLineID = mixer.dwLineID;
  controls.cControls = mixer.cControls;
  controls.cbmxctrl = sizeof (MIXERCONTROLA);
  controls.pamxctrl = &pamx;
  controls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
  mixerGetLineControlsA (this->card, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE);

  details.cbStruct = sizeof (MIXERCONTROLDETAILS);
  details.dwControlID = NULL;  // TODO: what value actually gets put here? does it matter?
  details.cChannels = mixer.cChannels;
  details.cbDetails = sizeof (DWORD);
  details.paDetails = &volume;
  details.cMultipleItems = 0;
  return mixerSetControlDetails (this->card, &details, MIXER_SETCONTROLDETAILSF_VALUE);
}

/**
 * Update all volume levels for the Win32 mixer on the current sound card.
 *
 * @address        0x00486BB4
 *
 * @param[in]      use_game_vol        TRUE to update with game volume,
 *                                     FALSE to update with user-provided volume.
 */
VOID SoundContext::MixerUpdateVolume (DWORD use_game_vol)
{
  MMRESULT result;

  MIXERLINEA mixer;
  MIXERCONTROLDETAILS details;
  MIXERLINECONTROLS controls;

  MixerVolume volume;

  if (use_game_vol) {
    // TODO: Indexing into an array with the value of a handle doesn't seem right.
    // What is this code actually doing?
    memcpy (&volume, &this->game_vol[this->card], sizeof (MixerVolume));
  } else {
    memcpy (&volume, &this->user_vol, sizeof (MixerVolume));
  }

  MixerUpdateComponentVolume (MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, volume.speaker_vol);
  MixerUpdateComponentVolume (MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC, volume.cd_vol);
  MixerUpdateComponentVolume (MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, volume.waveout_vol);

  // Update bass levels
  if (this->maybe_sound_enabled) {
    mixer.cbStruct = sizeof (MIXERLINEA);
    mixer.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    result = mixerGetLineInfoA (this->card, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE);
    if (MM_SUCCEEDED (result)) {
      MIXERCONTROLA pamx;
      controls.dwLineID = mixer.dwLineID;
      controls.cbStruct = sizeof (MIXERLINECONTROLSA);
      controls.cControls = mixer.cControls;
      controls.cbmxctrl = sizeof (MIXERCONTROLA);
      controls.pamxctrl = &pamx;
      controls.dwControlType = MIXERCONTROL_CONTROLTYPE_BASS;
      result = mixerGetLineControlsA (this->card, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE);
      if (MM_SUCCEEDED (result)) {
        details.cbStruct = sizeof (MIXERCONTROLDETAILS);
        details.dwControlID = NULL;  // TODO: what value actually gets put here? does it matter?
        details.cChannels = mixer.cChannels;
        details.cbDetails = sizeof (DWORD);
        details.cMultipleItems = 0;
        details.paDetails = &volume.bass_vol;
        result = mixerSetControlDetails (this->card, &details, MIXER_SETCONTROLDETAILSF_VALUE);
      } else {
        this->maybe_sound_enabled = 0;
      }
    } else {
      this->maybe_sound_enabled = 0;
    }
  }

  // Update treble levels
  if (this->maybe_sound_enabled) {
    mixer.cbStruct = sizeof (MIXERLINEA);
    mixer.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    result = mixerGetLineInfoA (this->card, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE);
    if (MM_SUCCEEDED (result)) {
      MIXERCONTROLA pamx;
      controls.dwLineID = mixer.dwLineID;
      controls.cbStruct = sizeof (MIXERLINECONTROLSA);
      controls.cControls = mixer.cControls;
      controls.cbmxctrl = sizeof (MIXERCONTROLA);
      controls.pamxctrl = &pamx;
      controls.dwControlType = MIXERCONTROL_CONTROLTYPE_TREBLE;
      result = mixerGetLineControlsA (this->card, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE);
      if (MM_SUCCEEDED (result)) {
        details.cbStruct = sizeof (MIXERCONTROLDETAILS);
        details.dwControlID = NULL;  // TODO: what value actually gets put here? does it matter?
        details.cChannels = mixer.cChannels;
        details.cbDetails = sizeof (DWORD);
        details.cMultipleItems = 0;
        details.paDetails = &volume.treble_vol;
        result = mixerSetControlDetails (this->card, &details, MIXER_SETCONTROLDETAILSF_VALUE);
      } else {
        this->maybe_sound_enabled = 0;
      }
    } else {
      this->maybe_sound_enabled = 0;
    }
  }
}

/**
 * Get the sound card and initialize volume mixer structures.
 *
 * @address        0x004868CC
 */
VOID SoundContext::MixerInit ()
{
  MIXERLINEA mixer;
  MIXERCONTROLA ctrl;
  MIXERLINECONTROLSA controls;
  MIXERCONTROLDETAILS details;
  MIXERCONTROLDETAILS_UNSIGNED det;

  if (REG_FAILED (TmRegistryQueryKeyDword (this->reg_key, "SoundCardID", (LPDWORD)this->card))) {
    this->card = 0;
    TmRegistrySetKeyDword (this->reg_key, "SoundCardID", (DWORD)this->card);
  }

  mixer.cbStruct = sizeof (MIXERLINEA);
  mixer.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
  if (MM_SUCCEEDED (mixerGetLineInfoA (this->card, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE))) {
    controls.dwLineID = mixer.dwLineID;
    controls.cControls = mixer.cControls;
    controls.cbmxctrl = sizeof (MIXERCONTROLA);
    controls.cbStruct = sizeof (MIXERLINECONTROLSA);
    controls.pamxctrl = &ctrl;
    controls.dwControlType = MIXERCONTROL_CONTROLTYPE_BASS;
    if (MM_SUCCEEDED (
          mixerGetLineControlsA (this->card, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE))) {
      details.dwControlID = ctrl.dwControlID;
      details.cbStruct = sizeof (MIXERCONTROLDETAILS);
      details.cChannels = mixer.cChannels;
      details.cbDetails = sizeof (MIXERCONTROLDETAILS_UNSIGNED);
      details.paDetails = &det;
      details.cMultipleItems = 0;
      mixerGetControlDetailsA (this->card, &details, MIXER_GETCONTROLDETAILSF_VALUE);
    } else {
      this->maybe_sound_enabled = 0;
    }
  } else {
    this->maybe_sound_enabled = 0;
  }

  int bufSize = sizeof (this->game_vol);
  if (REG_FAILED (
        TmRegistryQueryKeyRaw (this->reg_key, "GameVolume", (LPBYTE)this->game_vol, bufSize))) {
    memcpy (&this->game_vol[0], &this->user_vol, 20);
    memcpy (&this->game_vol[1], &this->user_vol, 20);
    memcpy (&this->game_vol[2], &this->user_vol, 20);
    MixerUseDefaultVolume ();
    TmRegistrySetKeyRaw (this->reg_key, "GameVolume", (LPBYTE)this->game_vol, bufSize);
  }
}