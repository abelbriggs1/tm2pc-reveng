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

#include "internal/win32_internal.h"

#include "tm2.h"

#include "sound_impl.hpp"

#include "sound.h"

#endif /* TM_SOUND_INTERNAL_H */