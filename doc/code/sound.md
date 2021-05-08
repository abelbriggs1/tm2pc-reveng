# Sound

TODO: There are a lot of `ifelse` statements dependent on a single boolean flag that
could probably be cleaned up into a more structured cleanup system (early returns?)

```c
void soundInit (TMSound * sound) { // @ 0x00486870
  sound->hwnd = getCurHwndThunk (); // @ 0x004A8F34
  openRegKey (sound->regKey, HKEY_LOCAL_MACHINE, 0);
  soundMixerInit (sound); // @ 0x004868CC
  setRegKeyBinary (sound->regKey, "UserVolume", sound->userVolume, 20);
  soundMixerInitVolumes (sound, 1);
}
```

```c
static void soundMixerInit (TMSound * sound) { // @ 0x004868CC
  MIXERLINEA mixer;
  MIXERCONTROLA ctrl;
  MIXERLINECONTROLSA controls;
  MIXERCONTROLDETAILS details;
  MIXERCONTROLDETAILS_UNSIGNED det;
  
  if (queryRegKeyDword (sound->regKey, "SoundCardID", sound->soundcardId)) { // @ 0x004AC5BC
    sound->soundcardId = 0;
    setRegKeyDword (sound->regKey, "SoundCardID", sound->soundCardId); // @ 0x004AC614
  }
  
  mixer.cbStruct = sizeof (MIXERLINEA);
  mixer.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
  if (mixerGetLineInfoA (sound->soundcardId, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE)) {
    sound->maybeSoundEnabled = 0;
  } else {
    controls.dwLineID = mixer.dwLineID;
    controls.cControls = mixer.cControls;
    controls.cbmxctrl = sizeof (MIXERCONTROLA);
    controls.cbStruct = sizeof (MIXERLINECONTROLSA);
    controls.pamxctrl = &ctrl;
    controls.dwControlType = MIXERCONTROL_CONTROLTYPE_BASS;
    if (mixerGetLineControlsA (sound->soundcardId, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE)) {
      sound->maybeSoundEnabled = 0;
    } else {
      details.dwControlId = ctrl.dwControlId;
      details.cbStruct = sizeof (MIXERCONTROLDETAILS);
      details.cChannels = mixer.cChannels;
      details.cbDetails = sizeof (MIXERCONTROLDETAILS_UNSIGNED);
      details.paDetails = &det;
      details.cMultipleItems = 0;
      mixerGetControlDetailsA (sound->soundcardId, &details, MIXER_GETCONTROLDETAILSF_VALUE);
    }
  }
  
  int bufSize = 60;
  if (queryRegKey (sound->regKey, "GameVolume", sound->gameVolume, &bufSize)) { // @ 0x004AC5E4
     memcpy (sound->gameVolume[0], sound->userVolume, 20);
     memcpy (sound->gameVolume[1], sound->userVolume, 20);
     memcpy (sound->gameVolume[2], sound->userVolume, 20);
     soundInitDefaultVolume (sound); // @ 0x00486B44
     setRegKeyBinary (sound->regKey, "GameVolume", sound->gameVolume, 60);
  }
}
```

```c
#define DEFAULT_VOLUME_CD_UNSIGNED   0x9999 // 60%
#define DEFAULT_VOLUME_WAVE_UNSIGNED 0xE666 // 90%

static void soundInitDefaultVolume (TMSound * sound) { // @ 0x00486B44
  sound->gameVolume[0].volumeWaveout = DEFAULT_VOLUME_WAVE_UNSIGNED;
  sound->gameVolume[0].volumeCd = DEFAULT_VOLUME_CD_UNSIGNED;
  sound->gameVolume[0].volumeBass = 0;
  sound->gameVolume[0].volumeTreble = 0;
  
  sound->gameVolume[1].volumeWaveout = DEFAULT_VOLUME_WAVE_UNSIGNED;
  sound->gameVolume[1].volumeCd = DEFAULT_VOLUME_CD_UNSIGNED;
  sound->gameVolume[1].volumeBass = 0;
  sound->gameVolume[1].volumeTreble = 0;
  
  sound->gameVolume[1].volumeWaveout = DEFAULT_VOLUME_WAVE_UNSIGNED;
  sound->gameVolume[1].volumeCd = DEFAULT_VOLUME_CD_UNSIGNED;
  sound->gameVolume[1].volumeBass = 0;
  sound->gameVolume[1].volumeTreble = 0;
}
```

```c
static void soundSetVolume (TMSound * sound, DWORD componentType, DWORD * volume) { // @ 0x00489340
  MIXERLINEA mixer;
  MIXERCONTROLDETAILS details;
  MIXERLINECONTROLS controls;
  
  mixer.dwComponentType = componentType;
  mixer.cbStruct = sizeof (MIXERLINEA);

  if (mixerGetLineInfoA (sound->soundcardId, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE) {
    if (componentType == MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC) {
      int dev = auxGetNumDevs();
      if (dev > 0) {
        do {
          dev--;
          mixer.cbStruct = sizeof (MIXERLINEA);
          mixerGetLineInfoA (dev, &mixer, MIXER_OBJECTF_AUX);
        } while (!(strstr (mixer.szName, "CD")) && i > 0);
      }
    }
  }
  
  int pamx;
  controls.dwLineID = mixer.dwLineID;
  controls.cControls = mixer.cControls;
  controls.cbmxctrl = sizeof (MIXERCONTROLA);
  controls.pamxctrl = &pamx;
  controls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
  mixerGetLineControlsA (sound->soundcardId, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE);
  
  details.cbStruct = sizeof (MIXERCONTROLDETAILS);
  details.dwControlID = NULL; // TODO: what value actually gets put here? does it matter?
  details.cChannels = mixer.cChannels;
  details.cbDetails = sizeof (DWORD);
  details.paDetails = &volume;
  details.cMultipleItems = 0;
  return mixerSetControlDetails (sound->soundcardId, &details, MIXER_SETCONTROLDETAILSF_VALUE);
}
```

```c
static MMRESULT soundMixerInitVolumes (TMSound * sound, int useGameVolume) { // @ 0x00486BB4
  MMRESULT result;

  MIXERLINEA mixer;
  MIXERCONTROLDETAILS details;
  MIXERLINECONTROLS controls;

  TMunkSoundComp component;

  if (useGameVolume) {
    memcpy (&component, sound->gameVolume[sound->soundcardId], sizeof (TMunkSoundComp));
  } else {
    memcpy (&component, sound->userVolume, sizeof (TMunkSoundComp));
  }
  
  soundSetVolume (sound, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS, &component.volumeSpeakers); // @ 0x00489340
  soundSetVolume (sound, MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC, &component.volumeCd); // @ 0x00489340
  soundSetVolume (sound, MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, &component.volumeWaveOut); // @ 0x00489340
  
  if (sound->maybeSoundEnabled) {
    // Bass
    mixer.cbStruct = sizeof (MIXERLINEA);
    mixer.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    result = mixerGetLineInfoA (sound->soundcardId, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE);
    if (result) {
      sound->maybeSoundEnabled = 0;
    } else {
      int pamx;
      controls.dwLineID = mixer.dwLineID;
      controls.cbStruct = sizeof (MIXERLINECONTROLSA);
      controls.cControls = mixer.cControls;
      controls.cbmxctrl = sizeof (MIXERCONTROLA);
      controls.pamxctrl = &pamx;
      controls.dwControlType = MIXERCONTROL_CONTROLTYPE_BASS;
      result = mixerGetLineControlsA (sound->soundcardId, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE);
      if (result) {
        sound->maybeSoundEnabled = 0;
      } else {
        details.cbStruct = sizeof (MIXERCONTROLDETAILS);
        details.dwControlID = NULL; // TODO: what value actually gets put here? does it matter?
        details.cChannels = mixer.cChannels;
        details.cbDetails = sizeof (DWORD);
        details.cMultipleItems = 0;
        details.paDetails = &component.volumeBass;
        result = mixerSetControlDetails (sound->soundcardId, &details, MIXER_SETCONTROLDETAILSF_VALUE);
      }
    }
    // Treble
    if (sound->maybeSoundEnabled) {
      mixer.cbStruct = sizeof (MIXERLINEA);
      mixer.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
      result = mixerGetLineInfoA (sound->soundcardId, &mixer, MIXER_GETLINEINFOF_COMPONENTTYPE);
      if (result) {
        sound->maybeSoundEnabled = 0;
      } else {
        int pamx;
        controls.dwLineID = mixer.dwLineID;
        controls.cbStruct = sizeof (MIXERLINECONTROLSA);
        controls.cControls = mixer.cControls;
        controls.cbmxctrl = sizeof (MIXERCONTROLA);
        controls.pamxctrl = &pamx;
        controls.dwControlType = MIXERCONTROL_CONTROLTYPE_TREBLE;
        result = mixerGetLineControlsA (sound->soundcardId, &controls, MIXER_GETLINECONTROLSF_ONEBYTYPE);
        if (result) {
          sound->maybeSoundEnabled = 0;
        } else {
          details.cbStruct = sizeof (MIXERCONTROLDETAILS);
          details.dwControlID = NULL; // TODO: what value actually gets put here? does it matter?
          details.cChannels = mixer.cChannels;
          details.cbDetails = sizeof (DWORD);
          details.cMultipleItems = 0;
          details.paDetails = &component.volumeTreble;
          result = mixerSetControlDetails (sound->soundcardId, &details, MIXER_SETCONTROLDETAILSF_VALUE);
        }
      }
    }
  }
  
  return result;
}
```