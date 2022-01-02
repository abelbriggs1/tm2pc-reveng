/**
 * @file graphics_internal.h
 *
 * Internal graphics header which exposes module-level variables and includes
 * for other graphics module files. This header should be included by all .c
 * files in the graphics module. Any new module-level header file should be
 * added to the includes in this file.
 */

#ifndef TM_GRAPHICS_INTERNAL_H
#define TM_GRAPHICS_INTERNAL_H

#include "internal/win32_internal.h"

#include <d3d.h>
#include <ddraw.h>

#include <stdint.h>

#include "tm2.h"

#include "graphics_impl.hpp"
#include "texture.h"

#include "graphics.h"

// Max number of video buffers.
#define MAX_BUFFERS 3

// Defined in graphics_impl.c
extern DWORD unk_graphics_init_flag;
extern ColorDepth color_depth;

#endif /* TM_GRAPHICS_INTERNAL_H */
