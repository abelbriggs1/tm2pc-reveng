/**
 * @file graphics.c
 *
 * Graphics functions.
 */

#include "graphics_internal.h"

#include "win32/error.h"
#include "win32/registry.h"
#include "win32/window.h"

/**
 * Commonly used flagsets for DirectDraw cooperative levels.
 * TODO: Give these better symbol names.
 */
#define COOP_NORMAL DDSCL_NORMAL
#define COOP_CHANGING DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN
#define COOP_FULL DDSCL_EXCLUSIVE | DDSCL_NOWINDOWCHANGES | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN

#define ARRAY_SIZE(arr) (sizeof (arr) / sizeof ((arr)[0]))

// TODO: Unknown why this isn't in WATCOM's headers. This definition
// is copied from Wine's `d3d.h`.
DEFINE_GUID (IID_IDirect3DHALDevice,
             0x84E63dE0,
             0x46AA,
             0x11CF,
             0x81,
             0x6F,
             0x00,
             0x00,
             0xC0,
             0x20,
             0x15,
             0x6E);

typedef struct {
  DWORD shade_mode;
  DWORD fill_mode;
  DWORD unk1;
  DWORD unk2;
  DWORD unk_render_state1;
  DWORD dither_mode;
  DWORD unk_render_state2;
  DWORD specular_enable;
  DWORD zbuf_enable;
  DWORD zbuf_write_enable;
  DWORD zbuf_func;
  DWORD dest_blend;
  DWORD unk_render_state3;
  DWORD alpha_blend_enable;
  DWORD unk3;
  DWORD unk4;
  DWORD unk5;
  DWORD unk6;
  DWORD unk7;
  DWORD unk8;
  DWORD unk9;
  DWORD unk10;
  DWORD unk11;
  DWORD unk12;
  DWORD unk13;
  DWORD unk14;
  DWORD unk15;
} Graphics3dProperties;  // total size 0x6C bytes

DWORD unk_graphics_init_flag;  // @address 0x00C85008

static DWORD coop_level;                        // @address 0x004E86AC
static Graphics3dProperties graphics_3d_props;  // @address 0x00C84F74
static DisplayCapabilities display_caps;        // @address 0x00C848E0
static UnkTextureStruct textures[512];          // @address 0x00A2DC48

/**
 * Release the Direct3D HAL device and its associated resources.
 *
 * @address        0x004B5EE8
 *
 * @param[in,out]  graphics            Graphics context.
 */
static VOID ReleaseHalDevice (TmGraphics* graphics)
{
  if (graphics->background) {
    IDirect3DMaterial2_Release (graphics->background);
    graphics->background = NULL;
  }
  if (graphics->unk_material) {
    IDirect3DMaterial2_Release (graphics->unk_material);
    graphics->unk_material = NULL;
  }
  if (graphics->viewport) {
    if (graphics->hal_device) {
      IDirect3DDevice2_DeleteViewport (graphics->hal_device, graphics->viewport);
    }
    IDirect3DViewport2_Release (graphics->viewport);
    graphics->viewport = NULL;
  }
  ReleaseUnkTextureStructs ();
  if (graphics->hal_device) {
    IDirect3DDevice2_Release (graphics->hal_device);
    graphics->hal_device = NULL;
  }
}

/**
 * Release texture structures.
 *
 * @address        0x00497828
 */
static VOID ReleaseUnkTextureStructs ()
{
  for (int i = 0; i < ARRAY_SIZE (textures); i++) {
    if (textures[i].unk1) {
      ReleaseUnkTextureStruct (&textures[i]);  // calls thunk version
    }
  }
}

/**
 * Create the graphics context's backbuffer.
 *
 * On error, this function raises structured exceptions which the caller
 * must handle.
 *
 * @address        0x004B48F4
 *
 * @param[in,out]  graphics           Graphics context.
 *
 */
static VOID CreateBackbufferSurface (TmGraphics* graphics)
{
  HRESULT result;
  DWORD surface_caps;

  // We try three times to create our surfaces, using various options for compatibility
  // reasons.
  if (display_caps.hw_no_2d_during_3d_scene) {
    result = E_FAIL;
  } else {
    surface_caps = unk_graphics_init_flag ? DDSCAPS_3DDEVICE : 0;
    surface_caps |= DDSCAPS_VIDEOMEMORY | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    result = CreateSurface (graphics, graphics->primary_surface, 0, 0, surface_caps, 2);
  }

  if (FAILED (result)) {
    // The host might not support triple buffering, so try with less backbuffers.
    surface_caps = unk_graphics_init_flag ? DDSCAPS_3DDEVICE : 0;
    surface_caps |= DDSCAPS_VIDEOMEMORY | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    result = CreateSurface (graphics, graphics->primary_surface, 0, 0, surface_caps, 1);
  }

  if (FAILED (result)) {
    // The host might not have a GPU with video memory, so make a last-ditch
    // effort to run without VRAM.
    surface_caps = unk_graphics_init_flag ? DDSCAPS_3DDEVICE : 0;
    surface_caps |= DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    result = CreateSurface (graphics, graphics->primary_surface, 0, 0, surface_caps, 2);
  }

  if (FAILED (result)) {
    // Your PC is hopeless.
    ReleaseSurfaces (graphics);
    TmErrorMessage (TM_ERROR_DRAW_CREATE_BACKBUFFER_FAIL, HRESULT_CODE (result));
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  DDSCAPS caps;
  memset (&caps, 0, sizeof (caps));
  caps.dwCaps = DDSCAPS_BACKBUFFER;
  result = IDirectDrawSurface2_GetAttachedSurface (graphics->primary_surface, &caps,
                                                   graphics->backbuffer_surface);
  if (FAILED (result)) {
    ReleaseSurfaces (graphics);
    TmErrorMessage (TM_ERROR_DRAW_NO_BACKBUFFER, HRESULT_CODE (result));
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  graphics->membuffer_surface = graphics->backbuffer_surface;
  graphics->unk_enum_val = 4;
  IDirectDrawSurface2_GetCaps (graphics->backbuffer_surface, &caps);
  if ((caps.dwCaps & DDSCAPS_VIDEOMEMORY) && display_caps.hw_bank_switched &&
      SUCCEEDED (CreateSurface (graphics, graphics->membuffer_surface, graphics->width,
                                graphics->height, DDSCAPS_SYSTEMMEMORY, 0))) {
    graphics->unk_enum_val = 3;
  }
}

/**
 * Create the graphics context's primary draw surface and memory buffer.
 *
 * On error, this function raises structured exceptions which the caller
 * must handle.
 *
 * @address        0x004B4B18
 *
 * @param[in,out]  graphics            Graphics context.
 */
static VOID CreatePrimaryAndMembufferSurfaces (TmGraphics* graphics)
{
  HRESULT result;

  DWORD caps = unk_graphics_init_flag ? DDSCAPS_3DDEVICE : 0;
  caps |= DDSCAPS_PRIMARYSURFACE;
  result = CreateSurface (graphics, graphics->primary_surface, 0, 0, caps, 0);
  if (FAILED (result)) {
    TmErrorMessage (TM_ERROR_DRAW_SURFACE_FAIL, HRESULT_CODE (result));
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  caps = unk_graphics_init_flag ? DDSCAPS_3DDEVICE : 0;
  caps |= DDSCAPS_SYSTEMMEMORY;
  result = CreateSurface (graphics, graphics->membuffer_surface, graphics->width, graphics->height,
                          caps, 0);
  if (FAILED (result)) {
    TmErrorMessage (TM_ERROR_DRAW_CREATE_MEMBUFFER_FAIL, HRESULT_CODE (result));
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  graphics->unk_enum_val = (graphics->unk_enum_val2 != 0);
}

/**
 * Initialize the graphics context's Direct3D functionality.
 *
 * @address        0x004B5BF0
 *
 * @param[in,out]  graphics            Graphics context.
 *
 * @return         DD_OK               Direct3D was successfully initialized.
 * @return         others              An error occurred during Direct3D initialization.
 */
static HRESULT InitDirect3d (TmGraphics* graphics)
{
  memset (&graphics_3d_props, 0, sizeof (graphics_3d_props));

  // TODO: what do these literal values mean?
  graphics_3d_props.unk1 = 0;
  graphics_3d_props.unk2 = 0;
  graphics_3d_props.unk_render_state1 = 0;
  graphics_3d_props.specular_enable = 0;
  graphics_3d_props.zbuf_enable = 0;
  graphics_3d_props.zbuf_write_enable = 0;
  graphics_3d_props.zbuf_func = 4;
  graphics_3d_props.dest_blend = 4;
  graphics_3d_props.alpha_blend_enable = 0;
  graphics_3d_props.unk13 = 0;
  graphics_3d_props.shade_mode = 1;
  graphics_3d_props.fill_mode = 3;
  graphics_3d_props.dither_mode = 1;
  graphics_3d_props.unk_render_state2 = 1;
  graphics_3d_props.unk_render_state3 = 2;
  graphics_3d_props.unk3 = 1;

  graphics->d3d2 = NULL;
  graphics->hal_device = NULL;
  graphics->viewport = NULL;
  graphics->unk3 = NULL;

  // TODO: Does this function's return value get used? The disassembly is
  // ambiguous.
  return IDirectDraw2_QueryInterface (graphics->ddraw2, &IID_IDirect3D2, graphics->d3d2);
}

/**
 * Initialize the graphics module.
 *
 * @address        0x004B3EAF
 *
 * @param[in,out]  graphics                  Graphics context.
 * @param[in]      unknown_graphics_flag     TODO: What is this flag?
 *
 * @return         DD_OK                     The graphics was initialized successfully.
 * @return         others                    An error occurred during initialization.
 */
HRESULT TmGraphicsInit (TmGraphics* graphics, DWORD unknown_graphics_flag)
{
  if (!graphics->ddraw2) {
    graphics->hwnd = TmWindowGetHwnd (&global_window);
    GetClientRect (graphics->hwnd, &graphics->rect);
    ClientToScreen (graphics->hwnd, &graphics->rect);
    ClientToScreen (graphics->hwnd, &graphics->rect.right);
    HDC temp = GetDC (graphics->hwnd);
    // These two calls to GetScreenMetrics() appear to be no-ops.
    GetScreenMetrics (SM_CXSCREEN);
    GetScreenMetrics (SM_CYSCREEN);
    graphics->maybe_bits_per_pixel2 = GetDeviceCaps (temp, BITSPIXEL);
    ReleaseDC (graphics->hwnd, temp);
  }

  // This also appears to be a no-op.
  TmRegistryUseKey (NULL);

  GUID ddraw_guid;
  memset (&ddraw_guid, 0, sizeof (ddraw_guid));
  memset (&graphics->d3d_hal_device_guid, 0, sizeof (graphics->d3d_hal_device_guid));
  graphics->graphics_id = 0;

  PHKEY key;
  if (REG_SUCCEEDED (TmRegistryOpenKey (key, HKEY_LOCAL_MACHINE, NULL))) {
    TmRegistryQueryKeyRaw (key, "DirectDraw GUID", &ddraw_guid, sizeof (ddraw_guid));
    TmRegistryQueryKeyDword (key, "Graphics", &graphics->graphics_id);
    TmRegistryCloseKey (key);
  }

  // If any of these are true, reinitialize all structures to a known-good state.
  if (unknown_graphics_flag || graphics->graphics_id || !graphics->ddraw2) {
    if (graphics->ddraw2) {
      ReleaseHalDevice (graphics);
      if (graphics->unk3) {
        // TODO: what is the D3D type here?
        IDirect3D2_Release (graphics->unk3);
        graphics->unk3 = NULL;
      }
      if (graphics->d3d2) {
        IDirect3D2_Release (graphics->d3d2);
        graphics->d3d2 = NULL;
      }
    }

    if (graphics->ddraw2 && coop_level != COOP_NORMAL) {
      coop_level = COOP_NORMAL;
      IDirectDraw2_SetCooperativeLevel (graphics->ddraw2, graphics->hwnd, COOP_NORMAL);
    }

    ReleaseSurfaces (graphics);

    if (graphics->ddraw2) {
      IDirectDraw2_RestoreDisplayMode (graphics->ddraw2);
      IDirectDraw2_Release (graphics->ddraw2);
      graphics->ddraw2 = NULL;
    }

    if (graphics->display_driver) {
      IDirectDraw_Release (graphics->display_driver);
      graphics->display_driver = NULL;
    }

    // TODO: This memcpy is inlined into the binary, but it seems to be
    // interleaved with user code, which confuses IDA's decompiler.
    // More analysis might be needed.
    memcpy (&graphics->d3d_hal_device_guid, &IID_IDirect3DHALDevice,
            sizeof (graphics->d3d_hal_device_guid));

    if (unknown_graphics_flag && graphics->graphics_id) {
      unk_graphics_init_flag = unknown_graphics_flag;
      graphics->unk_enum_val2 = unknown_graphics_flag;
    } else {
      unk_graphics_init_flag = 0;
    }

    // Test to see if the host supports our DirectDraw requirements and create our
    // DirectDraw2 object.
    if (!graphics->ddraw2) {
      HRESULT result = DirectDrawCreate (NULL, graphics->display_driver, NULL);
      // TODO: The code for initializing the display_driver jumps all over the place
      // and IDA's decompiler struggles with it - this section may need another analysis.
      if (SUCCEEDED (result)) {
        if (graphics->ddraw2) {
          if (coop_level == COOP_NORMAL) {
            result = DD_OK;
          } else {
            coop_level = COOP_NORMAL;
            result = IDirectDraw2_SetCooperativeLevel (graphics->ddraw2, graphics->hwnd,
                                                       COOP_NORMAL);
          }
        }
        if (SUCCEEDED (result)) {
          result = IDirectDraw_QueryInterface (graphics->display_driver, &IID_IDirectDraw2,
                                               graphics->ddraw2);
          if (graphics->ddraw2) {
            result = IDirectDraw_Release (graphics->display_driver);
            graphics->display_driver = NULL;
          }
        }
      }
      // We failed somewhere along the first attempt. Retry the test just to make sure.
      if (FAILED (result)) {
        // TODO: There might be another flag controlling the inside of this
        // if block.
        result = DirectDrawCreate (NULL, graphics->display_driver, NULL);
        if (graphics->ddraw2) {
          if (coop_level == COOP_NORMAL) {
            result = DD_OK;
          } else {
            coop_level = COOP_NORMAL;
            result = IDirectDraw2_SetCooperativeLevel (graphics->ddraw2, graphics->hwnd,
                                                       COOP_NORMAL);
          }
        }
        if (SUCCEEDED (result)) {
          result = IDirectDraw_QueryInterface (graphics->display_driver, &IID_IDirectDraw2,
                                               graphics->ddraw2);
          if (graphics->ddraw2) {
            result = IDirectDraw_Release (graphics->display_driver);
            graphics->display_driver = NULL;
          }
        }
      }
      if (FAILED (result)) {
        // Both attempts failed, so spit out an error message and error out.
        if (result == DDERR_NODIRECTDRAWSUPPORT) {
          TmErrorMessage (TM_ERROR_DRAW_UNSUPPORTED);
        } else if (result == DDERR_NODIRECTDRAWHW) {
          TmErrorMessage (TM_ERROR_DRAW_NO_HARDWARE);
        } else {
          TmErrorMessage (TM_ERROR_DRAW_CREATE_OBJ_FAIL, HRESULT_CODE (result));
        }
        return result;
      }
    }

    // Enumerate all fullscreen display modes
    if (graphics->ddraw2 && coop_level != COOP_FULL) {
      coop_level = COOP_FULL;
      IDirectDraw2_SetCooperativeLevel (graphics->ddraw2, graphics->hwnd, COOP_FULL);
    }
    display_caps.count = 0;
    IDirectDraw2_EnumDisplayModes (graphics->ddraw2, DDEDM_REFRESHRATES, NULL, &display_caps,
                                   &EnumDisplayModesCallback);
    if (graphics->ddraw2 && coop_level != COOP_NORMAL) {
      coop_level = COOP_NORMAL;
      IDirectDraw2_SetCooperativeLevel (graphics->ddraw2, graphics->hwnd, COOP_NORMAL);
    }
    qsort (display_caps.modes, display_caps.count, CompareDisplayModes);

    GetHardwareCapabilities (graphics, &display_caps);
    if (unk_graphics_init_flag) {
      InitDirect3d (graphics);
    }
  }
  return DD_OK;
}

/**
 * Render a frame by copying from the backbuffers to the primary surface.
 *
 * @address        0x004B5128
 *
 * @param[in,out]  graphics            Graphics context.
 *
 * @return         HRESULT             Error code from DirectDraw functions used to
 *                                     perform bit block transfers.
 */
HRESULT TmGraphicsRenderFrame (TmGraphics* graphics)
{
  // NB: The return value of this function is undefined if none of the
  // switch cases are hit.
  HRESULT result;

  if (!graphics->primary_surface) {
    return DD_OK;
  }

  if (IDirectDrawSurface2_IsLost (graphics->primary_surface) == DDERR_SURFACELOST) {
    RestoreSurfaces (graphics);
  }

  if (graphics->bltfx) {
    switch (graphics->unk_enum_val) {
      case 0:
        result = IDirectDrawSurface2_Blt (graphics->primary_surface, &graphics->rect,
                                          graphics->membuffer_surface, NULL, DDBLT_WAIT, NULL);
        break;
      case 1:
        result = IDirectDrawSurface2_Blt (graphics->primary_surface, NULL,
                                          graphics->membuffer_surface, NULL, DDBLT_WAIT, NULL);
        break;
      case 3:
        IDirectDrawSurface2_Blt (graphics->backbuffer_surface, NULL, graphics->membuffer_surface,
                                 NULL, DDBLT_WAIT, NULL);
        result = IDirectDrawSurface2_Flip (graphics->primary_surface, NULL, DDFLIP_WAIT);
        break;
      case 4:
        result = IDirectDrawSurface2_Flip (graphics->primary_surface, NULL, DDFLIP_WAIT);
        break;
    }
  } else {
    switch (graphics->unk_enum_val) {
      case 0:
        result = IDirectDrawSurface2_Blt (graphics->primary_surface, &graphics->rect,
                                          graphics->membuffer_surface, NULL, DDBLT_WAIT,
                                          graphics->bltfx);
        break;
      case 1:
      case 3:
      case 4:
        result = IDirectDrawSurface2_Blt (graphics->primary_surface, NULL,
                                          graphics->membuffer_surface, NULL, DDBLT_WAIT,
                                          graphics->bltfx);
        break;
    }
  }
  return result;
}

/**
 * Get the current display width.
 *
 * @address        0x004B5A14
 *
 * @param[in]      graphics            Graphics context.
 *
 * @return         DWORD               Display width, in pixels.
 */
DWORD TmGraphicsGetDisplayWidth (TmGraphics* graphics)
{
  return graphics->width;
}

/**
 * Get the current display height.
 *
 * @address        0x004B5A10
 *
 * @param[in]      graphics            Graphics context.
 *
 * @return         DWORD               Display height, in pixels.
 */
DWORD TmGraphicsGetDisplayHeight (TmGraphics* graphics)
{
  return graphics->height;
}

/**
 * Get the RGBA bit masks used for the graphics context's draw surface.
 *
 * @address        0x004B5A20
 *
 * @param[in]      graphics            Graphics context.
 * @param[out]     red_mask            Pointer to store red bit mask in. Optional.
 * @param[out]     green_mask          Pointer to store green bit mask in. Optional.
 * @param[out]     blue_mask           Pointer to store blue bit mask in. Optional.
 * @param[out]     alpha_mask          Pointer to store alpha bit mask in. Optional.
 */
VOID TmGraphicsGetColorDepthMasks (TmGraphics* graphics,
                                   DWORD* red_mask,
                                   DWORD* green_mask,
                                   DWORD* blue_mask,
                                   DWORD* alpha_mask)
{
  DDSURFACEDESC desc;
  memset (&desc, 0, sizeof (desc));
  desc.dwSize = sizeof (desc);

  IDirectDrawSurface2_GetSurfaceDesc (graphics->primary_surface, &desc);
  if (red_mask) {
    *red_mask = desc.ddpfPixelFormat.dwRBitMask;
  }
  if (green_mask) {
    *green_mask = desc.ddpfPixelFormat.dwGBitMask;
  }
  if (blue_mask) {
    *blue_mask = desc.ddpfPixelFormat.dwBBitMask;
  }
  if (alpha_mask) {
    *alpha_mask = desc.ddpfPixelFormat.dwRGBAlphaBitMask;
  }
}

/**
 * Set the effects for the graphics context to use on bit block transfers.
 *
 * @address        0x004B50F8
 *
 * @param[in,out]  graphics            Graphics context.
 * @param[in]      bltfx               Pointer to DirectDraw BLT effects object.
 */
VOID TmGraphicsSetBltfx (TmGraphics* graphics, LPDDBLTFX bltfx)
{
  graphics->bltfx = bltfx;
}

/**
 * Get the device context for a DirectDraw surface.
 *
 * @address        0x004B50A4
 *
 * @param[in,out]  graphics            Graphics context.
 * @param[in,out]  hdc                 Location where the device context will be placed.
 * @param[in]      surface             Surface which owns the device context. If NULL,
 *                                     the graphics's memory buffer will be used.
 *
 * @return         HRESULT             Status code from obtaining device context.
 */
HRESULT TmGraphicsGetSurfaceDeviceContext (TmGraphics* graphics,
                                           HDC hdc,
                                           LPDIRECTDRAWSURFACE2 surface)
{
  if (!surface) {
    surface = graphics->membuffer_surface;
  }
  return IDirectDrawSurface2_GetDC (surface, hdc);
}

/**
 * Release the device context for a DirectDraw surface.
 *
 * @address        0x004B50B8
 *
 * @param[in,out]  graphics            Graphics context.
 * @param[in]      hdc                 Device context to release.
 * @param[in,out]  surface             Surface which owns the device context. If NULL,
 *                                     the graphics's memory buffer will be used.
 *
 * @return         HRESULT             Status code from releasing device context.
 */
HRESULT TmGraphicsReleaseSurfaceDeviceContext (TmGraphics* graphics,
                                               HDC hdc,
                                               LPDIRECTDRAWSURFACE2 surface)
{
  if (!surface) {
    surface = graphics->membuffer_surface;
  }
  return IDirectDrawSurface2_ReleaseDC (surface, hdc);
}

/**
 * Change display modes and prepare to render frames to the window.
 *
 * On error, this function raises structured exceptions which the caller
 * must handle.
 *
 * @address        0x004B4C80
 *
 * @param[in,out]  graphics            Graphics context.
 */
VOID TmGraphicsDisplay (TmGraphics* graphics)
{
  HRESULT result;
  graphics->unk_enum_val2 = 1;

  ReleaseSurfaces (graphics);

  LONG window_desc = GetWindowLongA (graphics->hwnd, GWL_STYLE);
  SetWindowLongA (graphics->hwnd, GWL_STYLE,
                  window_desc & ~(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX));

  if (graphics->ddraw2) {
    if (coop_level == COOP_CHANGING) {
      result = DD_OK;
    } else {
      coop_level = COOP_CHANGING;
      result = IDirectDraw2_SetCooperativeLevel (graphics->ddraw2, graphics->hwnd, COOP_CHANGING);
    }
  } else {
    result = DD_OK;
  }

  if (FAILED (result) && result != DDERR_HWNDALREADYSET) {
    TmErrorMessage (TM_ERROR_DRAW_COOP_LEVEL_FAIL, HRESULT_CODE (result));
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  if (graphics->ddraw2) {
    IDirectDraw2_RestoreDisplayMode (graphics->ddraw2);
  }
  if (FAILED (IDirectDraw2_SetDisplayMode (graphics->ddraw2, graphics->width, graphics->height,
                                           graphics->maybe_bits_per_pixel, graphics->monitor_freq,
                                           NULL))) {
    TmErrorMessage (TM_ERROR_DRAW_UNSUPPORTED_VIDEO_MODE, graphics->width, graphics->height,
                    graphics->maybe_bits_per_pixel);
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  if (unk_graphics_init_flag) {
    CreateBackbufferSurface (graphics);
  } else {
    if (graphics->bltfx) {
      CreateBackbufferSurface (graphics);
    } else {
      CreatePrimaryAndMembufferSurfaces (graphics);
    }
  }
  CreateClipper (graphics);
}