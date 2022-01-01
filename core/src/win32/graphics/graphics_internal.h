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

// TM2 mostly uses D3D/DirectDraw2 structures, but was compiled
// with DX5.
#define DIRECT3D_VERSION 0x0500

#include <windows.h>

#include <d3d.h>
#include <ddraw.h>

#include "win32/graphics.h"

#include "clipper.h"
#include "display.h"
#include "surface.h"
#include "texture.h"

// Defined in graphics.c
extern DWORD unk_graphics_init_flag;

#endif /* TM_GRAPHICS_INTERNAL_H */
