/**
 * @file sound_internal.h
 *
 * Internal sound header which exposes module-level variables and includes
 * for other sound module files. This header should be included by all .c
 * files in the sound module. Any new module-level header file should be
 * added to the includes in this file.
 */

#ifndef TM_SOUND_INTERNAL_H
#define TM_SOUND_INTERNAL_H

#include <windows.h>

#include "tm2.h"

#include "internal/win32_internal.h"

#include "sound.h"

#include "mixer.h"
#include "sound_impl.h"

/**
 * Macros to determine if a Win32 Multimedia System call succeeded
 * or failed.
 */
#define MM_SUCCEEDED(code) ((code == MMSYSERR_NOERROR))
#define MM_FAILED(code) ((code != MMSYSERR_NOERROR))

#endif /* TM_SOUND_INTERNAL_H */