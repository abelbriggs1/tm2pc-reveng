/**
 * @file graphics_impl.c
 *
 * Internal implementation of graphics context.
 */

#include "graphics_internal.h"

/**
 * Commonly used flagsets for DirectDraw cooperative levels.
 * TODO: Give these better symbol names.
 */
#define COOP_NORMAL DDSCL_NORMAL
#define COOP_CHANGING DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN
#define COOP_FULL DDSCL_EXCLUSIVE | DDSCL_NOWINDOWCHANGES | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN

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

DWORD unk_graphics_init_flag;  // @address 0x00C85008
ColorDepth color_depth;        // @address 0x00A2DBA5

static DWORD coop_level;                        // @address 0x004E86AC
static Graphics3dProperties graphics_3d_props;  // @address 0x00C84F74
// TODO: Is this part of GraphicsContext?
static DisplayCapabilities display_caps;  // @address 0x00C848E0

/**
 * Release the Direct3D HAL device and its associated resources.
 *
 * @address        0x004B5EE8
 */
VOID GraphicsContext::ReleaseHalDevice ()
{
  if (this->background) {
    this->background->Release ();
    this->background = NULL;
  }
  if (this->unk_material) {
    this->unk_material->Release ();
    this->unk_material = NULL;
  }
  if (this->viewport) {
    if (this->hal_device) {
      this->hal_device->DeleteViewport (this->viewport);
    }
    this->viewport->Release ();
    this->viewport = NULL;
  }
  ReleaseUnkTextureStructs ();
  if (this->hal_device) {
    this->hal_device->Release ();
    this->hal_device = NULL;
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
 * @return         DD_OK              This function always returns DD_OK.
 */
HRESULT GraphicsContext::CreateBackbufferSurface ()
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
    result = CreateSurface (this->primary_surface, 0, 0, surface_caps, 2);
  }

  if (FAILED (result)) {
    // The host might not support triple buffering, so try with less backbuffers.
    surface_caps = unk_graphics_init_flag ? DDSCAPS_3DDEVICE : 0;
    surface_caps |= DDSCAPS_VIDEOMEMORY | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    result = CreateSurface (this->primary_surface, 0, 0, surface_caps, 1);
  }

  if (FAILED (result)) {
    // The host might not have a GPU with video memory, so make a last-ditch
    // effort to run without VRAM.
    surface_caps = unk_graphics_init_flag ? DDSCAPS_3DDEVICE : 0;
    surface_caps |= DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    result = CreateSurface (this->primary_surface, 0, 0, surface_caps, 2);
  }

  if (FAILED (result)) {
    // Your PC is hopeless.
    ReleaseSurfaces ();
    TmErrorMessage (TM_ERROR_DRAW_CREATE_BACKBUFFER_FAIL, HRESULT_CODE (result));
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  DDSCAPS caps;
  memset (&caps, 0, sizeof (caps));
  caps.dwCaps = DDSCAPS_BACKBUFFER;
  result = this->primary_surface->GetAttachedSurface (&caps, &this->backbuffer_surface);
  if (FAILED (result)) {
    ReleaseSurfaces ();
    TmErrorMessage (TM_ERROR_DRAW_NO_BACKBUFFER, HRESULT_CODE (result));
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  this->membuffer_surface = this->backbuffer_surface;
  this->unk_enum_val = 4;
  this->backbuffer_surface->GetCaps (&caps);
  if ((caps.dwCaps & DDSCAPS_VIDEOMEMORY) && display_caps.hw_bank_switched &&
      SUCCEEDED (CreateSurface (this->membuffer_surface, this->width, this->height,
                                DDSCAPS_SYSTEMMEMORY, 0))) {
    this->unk_enum_val = 3;
  }

  return DD_OK;
}

/**
 * Create the graphics context's primary draw surface and memory buffer.
 *
 * On error, this function raises structured exceptions which the caller
 * must handle.
 *
 * @address        0x004B4B18
 *
 * @return         DD_OK               This function always returns DD_OK.
 */
HRESULT GraphicsContext::CreatePrimaryAndMembufferSurfaces ()
{
  HRESULT result;

  DWORD caps = unk_graphics_init_flag ? DDSCAPS_3DDEVICE : 0;
  caps |= DDSCAPS_PRIMARYSURFACE;
  result = CreateSurface (this->primary_surface, 0, 0, caps, 0);
  if (FAILED (result)) {
    TmErrorMessage (TM_ERROR_DRAW_SURFACE_FAIL, HRESULT_CODE (result));
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  caps = unk_graphics_init_flag ? DDSCAPS_3DDEVICE : 0;
  caps |= DDSCAPS_SYSTEMMEMORY;
  result = CreateSurface (this->membuffer_surface, this->width, this->height, caps, 0);
  if (FAILED (result)) {
    TmErrorMessage (TM_ERROR_DRAW_CREATE_MEMBUFFER_FAIL, HRESULT_CODE (result));
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  this->unk_enum_val = (this->unk_enum_val2 != 0);

  return DD_OK;
}

/**
 * Initialize the graphics context's Direct3D functionality.
 *
 * @address        0x004B5BF0
 *
 * @return         DD_OK               Direct3D was successfully initialized.
 * @return         others              An error occurred during Direct3D initialization.
 */
HRESULT GraphicsContext::InitDirect3d ()
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

  this->d3d2 = NULL;
  this->hal_device = NULL;
  this->viewport = NULL;
  this->unk3 = NULL;

  // TODO: Does this function's return value get used? The disassembly is
  // ambiguous.
  return this->ddraw2->QueryInterface (IID_IDirect3D2, (LPVOID*)&this->d3d2);
}

/**
 * Change display modes and prepare to render frames to the window.
 *
 * On error, this function raises structured exceptions which the caller
 * must handle.
 *
 * @address        0x004B4C80
 *
 * @return         0                   Changing display modes was successful.
 * @return         others              An error occurred while changing display modes.
 */
HRESULT GraphicsContext::ChangeDisplayMode ()
{
  HRESULT result;
  this->unk_enum_val2 = 1;

  ReleaseSurfaces ();

  LONG window_desc = GetWindowLongA (this->hwnd, GWL_STYLE);
  SetWindowLongA (this->hwnd, GWL_STYLE, window_desc & ~(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX));

  if (this->ddraw2) {
    if (coop_level == COOP_CHANGING) {
      result = DD_OK;
    } else {
      coop_level = COOP_CHANGING;
      result = this->ddraw2->SetCooperativeLevel (this->hwnd, COOP_CHANGING);
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

  if (this->ddraw2) {
    this->ddraw2->RestoreDisplayMode ();
  }
  if (FAILED (this->ddraw2->SetDisplayMode (this->width, this->height, this->bits_per_pixel,
                                            this->monitor_freq, NULL))) {
    TmErrorMessage (TM_ERROR_DRAW_UNSUPPORTED_VIDEO_MODE, this->width, this->height,
                    this->bits_per_pixel);
    // TODO: Unwind is called here in the original binary - how do we
    // duplicate that?
    RaiseException (0, 0, 0, NULL);
  }

  if (unk_graphics_init_flag) {
    result = CreateBackbufferSurface ();
  } else {
    if (this->bltfx) {
      result = CreateBackbufferSurface ();
    } else {
      result = CreatePrimaryAndMembufferSurfaces ();
    }
  }
  CreateClipper ();
  return result;
}

/**
 * Adjust the window position and size according to the given arguments.
 *
 * @address        0x004B5A94
 *
 * @param[in,out]  graphics            Graphics context.
 * @param[in]      left                Horizontal position of window, in pixels.
 * @param[in]      top                 Vertical position of window, in pixels.
 * @param[in]      width               Width of window, in pixels.
 * @param[in]      height              Height of window, in pixels.
 */
VOID GraphicsContext::AdjustWindow (DWORD left, DWORD top, DWORD width, DWORD height)
{
  RECT rc;

  if (this->unk_enum_val2) {
    SetRect (&rc, 0, 0, width, height);
  } else {
    DWORD style = GetWindowLongA (this->hwnd, GWL_STYLE);
    SetRect (&rc, left, top, left + width, top + height);
    HMENU menu = GetMenu (this->hwnd);
    AdjustWindowRect (&rc, style, menu != NULL);
  }

  MoveWindow (this->hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

/**
 * Calculate the color depth of the primary surface and save it
 * into the global context.
 *
 * @address        0x0049628C
 *
 * @param[in]      graphics            Graphics context.
 */
VOID CalculateColorDepth (GraphicsContext* graphics)
{
  int red, green, blue;
  TmGraphicsGetColorDepthMasks (&red, &green, &blue, NULL);
  if (red == COLOR_MASK_R_15 && green == COLOR_MASK_G_15 && blue == COLOR_MASK_B_15) {
    color_depth = COLOR_DEPTH_15;
  } else if (red == COLOR_MASK_R_16 && green == COLOR_MASK_G_16 && blue == COLOR_MASK_B_16) {
    color_depth = COLOR_DEPTH_16;
  } else {
    color_depth = COLOR_DEPTH_UNKNOWN;
  }
}

/**
 * Initialize the graphics module.
 *
 * @address        0x004B3EAF
 *
 * @param[in]      unknown_graphics_flag     TODO: What is this flag?
 *
 * @return         DD_OK                     The graphics context was initialized successfully.
 * @return         others                    An error occurred during initialization.
 */
HRESULT GraphicsContext::Init (DWORD unknown_graphics_flag)
{
  if (!this->ddraw2) {
    this->hwnd = TmWindowGetHwnd ();  // inlined
    GetClientRect (this->hwnd, &this->rect);
    ClientToScreen (this->hwnd, (LPPOINT) & this->rect.left);
    ClientToScreen (this->hwnd, (LPPOINT) & this->rect.right);
    HDC temp = GetDC (this->hwnd);
    // These two calls to GetSystemMetrics() appear to be no-ops.
    GetSystemMetrics (SM_CXSCREEN);
    GetSystemMetrics (SM_CYSCREEN);
    this->maybe_bits_per_pixel2 = GetDeviceCaps (temp, BITSPIXEL);
    ReleaseDC (this->hwnd, temp);
  }

  // This also appears to be a no-op.
  TmRegistryUseKey (NULL);

  GUID ddraw_guid;
  memset (&ddraw_guid, 0, sizeof (ddraw_guid));
  memset (&this->d3d_hal_device_guid, 0, sizeof (this->d3d_hal_device_guid));
  this->graphics_id = 0;

  PHKEY key;
  if (REG_SUCCEEDED (TmRegistryOpenKey (key, HKEY_LOCAL_MACHINE, NULL))) {
    TmRegistryQueryKeyRaw (key, "DirectDraw GUID", (LPBYTE)&ddraw_guid, sizeof (ddraw_guid));
    TmRegistryQueryKeyDword (key, "Graphics", &this->graphics_id);
    TmRegistryCloseKey (key);
  }

  // If any of these are true, reinitialize all structures to a known-good state.
  if (unknown_graphics_flag || this->graphics_id || !this->ddraw2) {
    if (this->ddraw2) {
      ReleaseHalDevice ();
      if (this->unk3) {
        // TODO: what is the D3D type here?
        this->unk3->Release ();
        this->unk3 = NULL;
      }
      if (this->d3d2) {
        this->d3d2->Release ();
        this->d3d2 = NULL;
      }
    }

    if (this->ddraw2 && coop_level != COOP_NORMAL) {
      coop_level = COOP_NORMAL;
      this->ddraw2->SetCooperativeLevel (this->hwnd, COOP_NORMAL);
    }

    ReleaseSurfaces ();

    if (this->ddraw2) {
      this->ddraw2->RestoreDisplayMode ();
      this->ddraw2->Release ();
      this->ddraw2 = NULL;
    }

    if (this->display_driver) {
      this->display_driver->Release ();
      this->display_driver = NULL;
    }

    // TODO: This memcpy is inlined into the binary, but it seems to be
    // interleaved with user code, which confuses IDA's decompiler.
    // More analysis might be needed.
    memcpy (&this->d3d_hal_device_guid, &IID_IDirect3DHALDevice,
            sizeof (this->d3d_hal_device_guid));

    if (unknown_graphics_flag && this->graphics_id) {
      unk_graphics_init_flag = unknown_graphics_flag;
      this->unk_enum_val2 = unknown_graphics_flag;
    } else {
      unk_graphics_init_flag = 0;
    }

    // Test to see if the host supports our DirectDraw requirements and create our
    // DirectDraw2 object.
    if (!this->ddraw2) {
      HRESULT result = DirectDrawCreate (NULL, &this->display_driver, NULL);
      // TODO: The code for initializing the display_driver jumps all over the place
      // and IDA's decompiler struggles with it - this section may need another analysis.
      if (SUCCEEDED (result)) {
        if (this->ddraw2) {
          if (coop_level == COOP_NORMAL) {
            result = DD_OK;
          } else {
            coop_level = COOP_NORMAL;
            result = this->ddraw2->SetCooperativeLevel (this->hwnd, COOP_NORMAL);
          }
        }
        if (SUCCEEDED (result)) {
          result = this->display_driver->QueryInterface (IID_IDirectDraw2, (LPVOID*)&this->ddraw2);
          if (this->ddraw2) {
            result = IDirectDraw_Release (this->display_driver);
            this->display_driver = NULL;
          }
        }
      }
      // We failed somewhere along the first attempt. Retry the test just to make sure.
      if (FAILED (result)) {
        // TODO: There might be another flag controlling the inside of this
        // if block.
        result = DirectDrawCreate (NULL, &this->display_driver, NULL);
        if (this->ddraw2) {
          if (coop_level == COOP_NORMAL) {
            result = DD_OK;
          } else {
            coop_level = COOP_NORMAL;
            result = this->ddraw2->SetCooperativeLevel (this->hwnd, COOP_NORMAL);
          }
        }
        if (SUCCEEDED (result)) {
          result = this->display_driver->QueryInterface (IID_IDirectDraw2, (LPVOID*)&this->ddraw2);
          if (this->ddraw2) {
            result = this->display_driver->Release ();
            this->display_driver = NULL;
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
    if (this->ddraw2 && coop_level != COOP_FULL) {
      coop_level = COOP_FULL;
      this->ddraw2->SetCooperativeLevel (this->hwnd, COOP_FULL);
    }
    display_caps.count = 0;
    this->ddraw2->EnumDisplayModes (DDEDM_REFRESHRATES, NULL, &display_caps,
                                    &EnumDisplayModesCallback);
    if (this->ddraw2 && coop_level != COOP_NORMAL) {
      coop_level = COOP_NORMAL;
      this->ddraw2->SetCooperativeLevel (this->hwnd, COOP_NORMAL);
    }
    qsort (display_caps.modes, display_caps.count, sizeof (display_caps.modes[0]),
           CompareDisplayModes);

    GetHardwareCapabilities (&display_caps);
    if (unk_graphics_init_flag) {
      InitDirect3d ();
    }
  }
  return DD_OK;
}

/**
 * Swap a backbuffer with the primary surface.
 *
 * @address        0x004B5128
 *
 * @return         HRESULT             Error code from DirectDraw functions used to
 *                                     perform bit block transfers.
 */
HRESULT GraphicsContext::SwapBuffers ()
{
  // NB: The return value of this function is undefined if none of the
  // switch cases are hit.
  HRESULT result;

  if (!this->primary_surface) {
    return DD_OK;
  }

  if (this->primary_surface->IsLost () == DDERR_SURFACELOST) {
    RestoreSurfaces ();
  }

  if (this->bltfx) {
    switch (this->unk_enum_val) {
      case 0:
        result = this->primary_surface->Blt (&this->rect, this->membuffer_surface, NULL, DDBLT_WAIT,
                                             NULL);
        break;
      case 1:
        result = this->primary_surface->Blt (NULL, this->membuffer_surface, NULL, DDBLT_WAIT, NULL);
        break;
      case 3:
        this->backbuffer_surface->Blt (NULL, this->membuffer_surface, NULL, DDBLT_WAIT, NULL);
        result = this->primary_surface->Flip (NULL, DDFLIP_WAIT);
        break;
      case 4:
        result = this->primary_surface->Flip (NULL, DDFLIP_WAIT);
        break;
    }
  } else {
    switch (this->unk_enum_val) {
      case 0:
        result = this->primary_surface->Blt (&this->rect, this->membuffer_surface, NULL, DDBLT_WAIT,
                                             this->bltfx);
        break;
      case 1:
      case 3:
      case 4:
        result = this->primary_surface->Blt (NULL, this->membuffer_surface, NULL, DDBLT_WAIT,
                                             this->bltfx);
        break;
    }
  }
  return result;
}

/**
 * Fill a DirectDraw surface buffer with the specified color.
 *
 * @address        0x004B4DA3
 *
 * @param[in]      color               16-bit RGB color to use.
 * @param[in,out]  surface             Surface to fill. If NULL, the graphics's memory
 *                                     buffer surface will be used if it exists.
 *
 * @return         DD_OK               The fill was successful.
 * @return         E_FAIL              surface was NULL, and the graphics's memory buffer
 *                                     was uninitialized.
 * @return         others              Error code from DirectDraw fill operation.
 */
HRESULT GraphicsContext::FillBuffer (DWORD color, LPDIRECTDRAWSURFACE2 surface)
{
  if (!surface) {
    surface = this->membuffer_surface;
    if (!surface) {
      return E_FAIL;
    }
  }

  DDBLTFX fx;
  memset (&fx, 0, sizeof (fx));
  fx.dwFillColor = color;
  fx.dwSize = sizeof (fx);
  return surface->Blt (NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &fx);
}

/**
 * Get the current display width.
 *
 * @address        0x004B5A14
 *
 * @return         DWORD               Display width, in pixels.
 */
DWORD GraphicsContext::GetDisplayWidth ()
{
  return this->width;
}

/**
 * Get the current display height.
 *
 * @address        0x004B5A10
 *
 * @return         DWORD               Display height, in pixels.
 */
DWORD GraphicsContext::GetDisplayHeight ()
{
  return this->height;
}

/**
 * Get the RGBA bit masks used for the graphics context's draw surface.
 *
 * @address        0x004B5A20
 *
 * @param[out]     red_mask            Pointer to store red bit mask in. Optional.
 * @param[out]     green_mask          Pointer to store green bit mask in. Optional.
 * @param[out]     blue_mask           Pointer to store blue bit mask in. Optional.
 * @param[out]     alpha_mask          Pointer to store alpha bit mask in. Optional.
 */
VOID GraphicsContext::GetColorDepthMasks (DWORD* red_mask,
                                          DWORD* green_mask,
                                          DWORD* blue_mask,
                                          DWORD* alpha_mask)
{
  DDSURFACEDESC desc;
  memset (&desc, 0, sizeof (desc));
  desc.dwSize = sizeof (desc);

  this->primary_surface->GetSurfaceDesc (&desc);
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
 * @param[in]      bltfx               Pointer to DirectDraw BLT effects object.
 */
VOID GraphicsContext::SetBltfx (LPDDBLTFX bltfx)
{
  this->bltfx = bltfx;
}

/**
 * Get the device context for a DirectDraw surface.
 *
 * @address        0x004B50A4
 *
 * @param[out]     hdc                 Location where the device context will be placed.
 * @param[in]      surface             Surface which owns the device context. If NULL,
 *                                     the graphics's memory buffer will be used.
 *
 * @return         HRESULT             Status code from obtaining device context.
 */
HRESULT GraphicsContext::GetSurfaceDeviceContext (HDC* hdc, LPDIRECTDRAWSURFACE2 surface)
{
  if (!surface) {
    surface = this->membuffer_surface;
  }
  return surface->GetDC (hdc);
}

/**
 * Release the device context for a DirectDraw surface.
 *
 * @address        0x004B50B8
 *
 * @param[in]      hdc                 Device context to release.
 * @param[in,out]  surface             Surface which owns the device context. If NULL,
 *                                     the graphics's memory buffer will be used.
 *
 * @return         HRESULT             Status code from releasing device context.
 */
HRESULT GraphicsContext::ReleaseSurfaceDeviceContext (HDC hdc, LPDIRECTDRAWSURFACE2 surface)
{
  if (!surface) {
    surface = this->membuffer_surface;
  }
  return surface->ReleaseDC (hdc);
}

HRESULT GraphicsContext::SetDisplayMode (DWORD width,
                                         DWORD height,
                                         DWORD bits_per_pixel,
                                         DWORD monitor_freq)
{  // @address 0x004B45A0
  HRESULT result;
  // TODO: It's odd that an internal graphics function calls the global
  // function. Is this really a GraphicsContext function?
  TmGraphicsFillFramebuffers (0);

  if (unk_graphics_init_flag) {
    ReleaseHalDevice ();
  }
  ReleaseSurfaces ();

  int mode_index = 0;
  while (mode_index < display_caps.count) {
    DisplayMode* mode = &display_caps.modes[mode_index];
    if (width == mode->width && height == mode->height && bits_per_pixel == mode->rgb_bit_count &&
        (!monitor_freq || monitor_freq == mode->monitor_freq)) {
      break;
    }
    mode_index++;
  }

  this->display_mode_index = mode_index;
  this->width = width;
  this->height = height;
  this->bits_per_pixel = bits_per_pixel;
  this->monitor_freq = monitor_freq;

  this->rect.right = this->width + this->rect.left;
  this->rect.bottom = this->height + this->rect.top;

  if (this->unk_enum_val2) {
    result = ChangeDisplayMode ();
    if (SUCCEEDED (result)) {
      // TODO: Turn on or off the mouse cursor and set this->cursor_visible
      // based on an unknown value @address 0x00A5D370.
    }
  } else {
    LONG window_desc = GetWindowLongA (this->hwnd, GWL_STYLE);
    SetWindowLongA (this->hwnd, GWL_STYLE, window_desc & ~(WS_POPUP));

    if (this->ddraw2) {
      if (coop_level == COOP_NORMAL) {
        result = DD_OK;
      } else {
        coop_level = COOP_NORMAL;
        result = this->ddraw2->SetCooperativeLevel (this->hwnd, COOP_CHANGING);
      }
    } else {
      result = DD_OK;
    }

    if (FAILED (result)) {
      TmErrorMessage (TM_ERROR_DRAW_COOP_LEVEL_FAIL, HRESULT_CODE (result));
      // TODO: Unwind is called here in the original binary - how do we
      // duplicate that?
      RaiseException (0, 0, 0, NULL);
    }

    ReleaseSurfaces ();

    if (this->ddraw2) {
      this->ddraw2->RestoreDisplayMode ();
    }
    result = CreatePrimaryAndMembufferSurfaces ();
    CreateClipper ();
  }

  AdjustWindow (this->rect.left, this->rect.top, this->width, this->height);
  CalculateColorDepth ();
  TmGraphicsFillFramebuffers (0);

  if (SUCCEEDED (result)) {
  }
}