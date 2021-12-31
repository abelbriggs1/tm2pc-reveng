/**
 * @file renderer_internal.h
 *
 * Internal renderer header which exposes module-level variables and includes.
 * This header should be included by all .c files in the renderer module.
 * Any new module-level header file should be added to the includes in
 * this file.
 */

#ifndef TM_RENDERER_INTERNAL_H
#define TM_RENDERER_INTERNAL_H

// TM2 mostly uses D3D/DirectDraw2 structures, but was compiled
// with DX5.
#define DIRECT3D_VERSION 0x0500

#include <windows.h>

#include <d3d.h>
#include <ddraw.h>

#include "win32/renderer.h"

#include "clipper.h"
#include "display.h"
#include "surface.h"
#include "texture.h"

// Defined in renderer.c
extern int unk_renderer_init_flag;

#endif /* TM_RENDERER_INTERNAL_H */
