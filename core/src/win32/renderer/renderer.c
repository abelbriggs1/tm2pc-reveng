/**
 * @file renderer.c
 *
 * Renderer functions.
 */

#include <windows.h>

#define DIRECT3D_VERSION 0x0500

#include <d3d.h>
#include <ddraw.h>

#include "win32/error.h"
#include "win32/registry.h"
#include "win32/window.h"

#include "win32/renderer.h"

/**
 * Commonly used flagsets for DirectDraw cooperative levels.
 * TODO: Give these better symbol names.
 */
#define COOP_NORMAL DDSCL_NORMAL
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
  int width;
  int height;
  int rgb_bit_count;
  int monitor_freq;
} DisplayMode;

typedef struct {
  D3DDEVICEDESC hw_desc;
  D3DDEVICEDESC hel_desc;
} DisplayDescriptors;

typedef struct {
  int count;
  DisplayMode modes[30];
  DDCAPS_DX5 hw_caps;
  DDCAPS_DX5 hel_caps;
  int hw_supports_3d;
  int hw_bank_switched;
  int hw_blt_stretched;
  int hw_no_2d_during_3d_scene;
  DisplayDescriptors descriptors;
} DisplayCapabilities;

typedef struct {
  int shade_mode;
  int fill_mode;
  int unk1;
  int unk2;
  int unk_render_state1;
  int dither_mode;
  int unk_render_state2;
  int specular_enable;
  int zbuf_enable;
  int zbuf_write_enable;
  int zbuf_func;
  int dest_blend;
  int unk_render_state3;
  int alpha_blend_enable;
  int unk3;
  int unk4;
  int unk5;
  int unk6;
  int unk7;
  int unk8;
  int unk9;
  int unk10;
  int unk11;
  int unk12;
  int unk13;
  int unk14;
  int unk15;
} Renderer3dProperties;  // total size 0x6C bytes

typedef struct {
  // TODO: These are pointers to D3D types.
  int unk1;
  int unk2;
  int unk3;
  int unk4;
  int unk5;
  int unk6;
} UnkTextureStruct;

static int coop_level;                          // @address 0x004E86AC
static int unk_renderer_init_flag;              // @address 0x00C85008
static Renderer3dProperties renderer_3d_props;  // @address 0x00C84F74
static DisplayCapabilities display_caps;        // @address 0x00C848E0
static UnkTextureStruct textures[512];          // @address 0x00A2DC48

/**
 * Release the Direct3D HAL device and its associated resources.
 *
 * @address        0x004B5EE8
 *
 * @param[in,out]  renderer            Renderer context.
 */
static void ReleaseHalDevice (TmRenderer* renderer)
{
  if (renderer->background) {
    IDirect3DMaterial2_Release (renderer->background);
    renderer->background = NULL;
  }
  if (renderer->unk_material) {
    IDirect3DMaterial2_Release (renderer->unk_material);
    renderer->unk_material = NULL;
  }
  if (renderer->viewport) {
    if (renderer->hal_device) {
      IDirect3DDevice2_DeleteViewport (renderer->hal_device, renderer->viewport);
    }
    IDirect3DViewport2_Release (renderer->viewport);
    renderer->viewport = NULL;
  }
  ReleaseUnkTextureStructs ();
  if (renderer->hal_device) {
    IDirect3DDevice2_Release (renderer->hal_device);
    renderer->hal_device = NULL;
  }
}

static void ReleaseUnkTextureStructs ()
{  // @address 0x00497828
  for (int i = 0; i < ARRAY_SIZE (textures); i++) {
    if (textures[i].unk1) {
      ReleaseUnkTextureStruct (&textures[i]);  // calls thunk
    }
  }
}

/**
 * Release a texture's Direct3D resources.
 *
 * @address        0x004C1024
 *                 0x004C101C (thunk)
 *
 * @param[in,out]  texture             Texture to release.
 */
static void ReleaseUnkTextureStruct (UnkTextureStruct* texture)
{
  // TODO: This function will need to be combed once UnkTextureStruct
  // has been filled with the proper types. The current implementation
  // is a guess based on patterns from other release functions.
  if (texture->unk3) {
    LPDIRECT3DTEXTURE tex;
    (*(*texture->unk4 + 108)) (texture->unk4);  // unknown D3D function call
    IDirect3D2_QueryInterface (texture->unk3, &IID_IDirect3DTexture, tex);
    if (tex) {
      (*(*tex + 28)) (tex);  // unknown D3D function call
      IDirect3DTexture_Release (tex);
    }
    IDirect3D2_Release (texture->unk3);
    texture->unk3 = NULL;
  }

  if (texture->unk4) {
    IDirect3D2_Release (texture->unk4);
    texture->unk4 = NULL;
  }

  if (texture->unk5) {
    IDirect3D2_Release (texture->unk5);
    texture->unk5 = NULL;
  }

  texture->unk1 = NULL;
}

/**
 * Given a DirectDraw surface descriptor which describes a display mode,
 * determine if the display mode is valid and add it to the renderer's
 * list of enumerated display modes if so.
 *
 * This function is a callback for IDirectDraw2_EnumDisplayModes and
 * should not be called by normal means.
 *
 * @address        0x004B592C
 *
 * @param[in]      desc                Surface descriptor which describes a display mode.
 * @param[in,out]  internal            Internal renderer display capabilities structure.
 *
 * @return         DDENUMRET_OK        This function always returns DDENUMRET_OK.
 */
static HRESULT EnumDisplayModesCallback (LPDDSURFACEDESC2 desc, LPVOID internal)
{
  DisplayCapabilities* caps = (DisplayCapabilities*)internal;

  if (desc->ddpfPixelFormat.dwRGBBitCount == 16) {
    int max_width = unk_renderer_init_flag ? 800 : 640;
    if (max_width >= desc->dwWidth) {
      caps->modes[caps->count].width = desc->dwWidth;
      caps->modes[caps->count].height = desc->dwHeight;
      caps->modes[caps->count].rgb_bit_count = desc->ddpfPixelFormat.dwRGBBitCount;
      caps->modes[caps->count].monitor_freq = desc->dwRefreshRate;
      caps->count++;
    }
  }
  return DDENUMRET_OK;
}

/**
 * Given two display modes, compare their internal values to determine
 * if they are identical.
 *
 * This function is a callback for qsort() and should not be called
 * by normal means.
 *
 * @address        0x004B584C
 *
 * @param[in]      arg1                First display mode to compare.
 * @param[in]      arg2                Second display mode to compare.
 *
 * @return         0                   The two display modes are identical.
 * @return         positive            The first display mode is greater.
 * @return         negative            The second display mode is greater.
 */
static int CompareDisplayModes (const void* arg1, const void* arg2)
{
  DisplayMode* lhs = (DisplayMode*)arg1;
  DisplayMode* rhs = (DisplayMode*)arg2;

  int result = lhs->rgb_bit_count - rhs->rgb_bit_count;
  if (!result) {
    result = lhs->width - rhs->width;
    if (!result) {
      result = lhs->height - rhs->height;
    }
  }
  return result;
}

/**
 * Retrieve DirectDraw hardware capabilities and save them in an internal
 * renderer structure for later use.
 *
 * @address        0x004B5874
 *
 * @param[in]      renderer            Renderer context.
 * @param[in,out]  caps                Internal capabilities structure.
 *
 * @return         DD_OK               Hardware capabilities were successfully retrieved.
 * @return         other               An error occurred retrieving hardware capabilities.
 */
static HRESULT GetHardwareCapabilities (TmRenderer* renderer, DisplayCapabilities* caps)
{
  memset (&caps->hw_caps, 0, sizeof (caps->hw_caps));
  caps->hw_caps.dwSize = sizeof (caps->hw_caps);
  memset (&caps->hel_caps, 0, sizeof (caps->hel_caps));
  caps->hel_caps.dwSize = sizeof (caps->hw_caps);

  HRESULT result = IDirectDraw2_GetCaps (renderer->ddraw2, &caps->hw_caps, &caps->hel_caps);
  if (SUCCEEDED (result)) {
    caps->hw_supports_3d = (caps->hw_caps.dwCaps & DDCAPS_3D) != 0;
    caps->hw_bank_switched = (caps->hw_caps.dwCaps & DDCAPS_BANKSWITCHED) != 0;
    caps->hw_blt_stretched = (caps->hw_caps.dwCaps & DDCAPS_BLTSTRETCH) != 0;
    caps->hw_no_2d_during_3d_scene = (caps->hw_caps.dwCaps2 & DDCAPS2_NO2DDURING3DSCENE) != 0;
    result = DD_OK;
  }
  return result;
}

/**
 * Initialize the renderer's Direct3D functionality.
 *
 * @address        0x004B5BF0
 *
 * @param[in,out]  renderer            Renderer context.
 *
 * @return         DD_OK               Direct3D was successfully initialized.
 * @return         others              An error occurred during Direct3D initialization.
 */
static HRESULT InitDirect3d (TmRenderer* renderer)
{
  memset (&renderer_3d_props, 0, sizeof (renderer_3d_props));

  // TODO: what do these literal values mean?
  renderer_3d_props.unk1 = 0;
  renderer_3d_props.unk2 = 0;
  renderer_3d_props.unk_render_state1 = 0;
  renderer_3d_props.specular_enable = 0;
  renderer_3d_props.zbuf_enable = 0;
  renderer_3d_props.zbuf_write_enable = 0;
  renderer_3d_props.zbuf_func = 4;
  renderer_3d_props.dest_blend = 4;
  renderer_3d_props.alpha_blend_enable = 0;
  renderer_3d_props.unk13 = 0;
  renderer_3d_props.shade_mode = 1;
  renderer_3d_props.fill_mode = 3;
  renderer_3d_props.dither_mode = 1;
  renderer_3d_props.unk_render_state2 = 1;
  renderer_3d_props.unk_render_state3 = 2;
  renderer_3d_props.unk3 = 1;

  renderer->d3d2 = NULL;
  renderer->hal_device = NULL;
  renderer->viewport = NULL;
  renderer->unk3 = NULL;

  // TODO: Does this function's return value get used? The disassembly is
  // ambiguous.
  return IDirectDraw2_QueryInterface (renderer->ddraw2, &IID_IDirect3D2, renderer->d3d2);
}

/**
 * Initialize the renderer module.
 *
 * @address        0x004B3EAF
 *
 * @param[in,out]  renderer                  Renderer context.
 * @param[in]      unknown_renderer_flag     TODO: What is this flag?
 *
 * @return         DD_OK                     The renderer was initialized successfully.
 * @return         others                    An error occurred during initialization.
 */
HRESULT TmRendererInit (TmRenderer* renderer, int unknown_renderer_flag)
{
  if (!renderer->ddraw2) {
    renderer->hwnd = TmWindowGetHwnd (&global_window);
    GetClientRect (renderer->hwnd, &renderer->rect);
    ClientToScreen (renderer->hwnd, &renderer->rect);
    ClientToScreen (renderer->hwnd, &renderer->rect.right);
    HDC temp = GetDC (renderer->hwnd);
    // These two calls to GetScreenMetrics() appear to be no-ops.
    GetScreenMetrics (SM_CXSCREEN);
    GetScreenMetrics (SM_CYSCREEN);
    renderer->maybe_bits_per_pixel2 = GetDeviceCaps (temp, BITSPIXEL);
    ReleaseDC (renderer->hwnd, temp);
  }

  // This also appears to be a no-op.
  TmRegistryUseKey (NULL);

  GUID ddraw_guid;
  memset (&ddraw_guid, 0, sizeof (ddraw_guid));
  memset (&renderer->d3d_hal_device_guid, 0, sizeof (renderer->d3d_hal_device_guid));
  renderer->renderer_id = 0;

  PHKEY key;
  if (REG_SUCCEEDED (TmRegistryOpenKey (key, HKEY_LOCAL_MACHINE, NULL))) {
    TmRegistryQueryKeyRaw (key, "DirectDraw GUID", &ddraw_guid, sizeof (ddraw_guid));
    TmRegistryQueryKeyDword (key, "Renderer", &renderer->renderer_id);
    TmRegistryCloseKey (key);
  }

  // If any of these are true, reinitialize all structures to a known-good state.
  if (unknown_renderer_flag || renderer->renderer_id || !renderer->ddraw2) {
    if (renderer->ddraw2) {
      ReleaseHalDevice (renderer);
      if (renderer->unk3) {
        // TODO: what is the D3D type here?
        IDirect3D2_Release (renderer->unk3);
        renderer->unk3 = NULL;
      }
      if (renderer->d3d2) {
        IDirect3D2_Release (renderer->d3d2);
        renderer->d3d2 = NULL;
      }
    }

    if (renderer->ddraw2 && coop_level != COOP_NORMAL) {
      coop_level = COOP_NORMAL;
      IDirectDraw2_SetCooperativeLevel (renderer->ddraw2, renderer->hwnd, COOP_NORMAL);
    }

    ReleaseSurfaces (renderer);

    if (renderer->ddraw2) {
      IDirectDraw2_RestoreDisplayMode (renderer->ddraw2);
      IDirectDraw2_Release (renderer->ddraw2);
      renderer->ddraw2 = NULL;
    }

    if (renderer->display_driver) {
      IDirectDraw_Release (renderer->display_driver);
      renderer->display_driver = NULL;
    }

    // TODO: This memcpy is inlined into the binary, but it seems to be
    // interleaved with user code, which confuses IDA's decompiler.
    // More analysis might be needed.
    memcpy (&renderer->d3d_hal_device_guid, &IID_IDirect3DHALDevice,
            sizeof (renderer->d3d_hal_device_guid));

    if (unknown_renderer_flag && renderer->renderer_id) {
      unk_renderer_init_flag = unknown_renderer_flag;
      renderer->unk_enum_val2 = unknown_renderer_flag;
    } else {
      unk_renderer_init_flag = 0;
    }

    // Test to see if the host supports our DirectDraw requirements and create our
    // DirectDraw2 object.
    if (!renderer->ddraw2) {
      HRESULT result = DirectDrawCreate (NULL, renderer->display_driver, NULL);
      // TODO: The code for initializing the display_driver jumps all over the place
      // and IDA's decompiler struggles with it - this section may need another analysis.
      if (SUCCEEDED (result)) {
        if (renderer->ddraw2) {
          if (coop_level == COOP_NORMAL) {
            result = DD_OK;
          } else {
            coop_level = COOP_NORMAL;
            result = IDirectDraw2_SetCooperativeLevel (renderer->ddraw2, renderer->hwnd,
                                                       COOP_NORMAL);
          }
        }
        if (SUCCEEDED (result)) {
          result = IDirectDraw_QueryInterface (renderer->display_driver, &IID_IDirectDraw2,
                                               renderer->ddraw2);
          if (renderer->ddraw2) {
            result = IDirectDraw_Release (renderer->display_driver);
            renderer->display_driver = NULL;
          }
        }
      }
      // We failed somewhere along the first attempt. Retry the test just to make sure.
      if (FAILED (result)) {
        // TODO: There might be another flag controlling the inside of this
        // if block.
        result = DirectDrawCreate (NULL, renderer->display_driver, NULL);
        if (renderer->ddraw2) {
          if (coop_level == COOP_NORMAL) {
            result = DD_OK;
          } else {
            coop_level = COOP_NORMAL;
            result = IDirectDraw2_SetCooperativeLevel (renderer->ddraw2, renderer->hwnd,
                                                       COOP_NORMAL);
          }
        }
        if (SUCCEEDED (result)) {
          result = IDirectDraw_QueryInterface (renderer->display_driver, &IID_IDirectDraw2,
                                               renderer->ddraw2);
          if (renderer->ddraw2) {
            result = IDirectDraw_Release (renderer->display_driver);
            renderer->display_driver = NULL;
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
    if (renderer->ddraw2 && coop_level != COOP_FULL) {
      coop_level = COOP_FULL;
      IDirectDraw2_SetCooperativeLevel (renderer->ddraw2, renderer->hwnd, COOP_FULL);
    }
    display_caps.count = 0;
    IDirectDraw2_EnumDisplayModes (renderer->ddraw2, DDEDM_REFRESHRATES, NULL, &display_caps,
                                   &EnumDisplayModesCallback);
    if (renderer->ddraw2 && coop_level != COOP_NORMAL) {
      coop_level = COOP_NORMAL;
      IDirectDraw2_SetCooperativeLevel (renderer->ddraw2, renderer->hwnd, COOP_NORMAL);
    }
    qsort (display_caps.modes, display_caps.count, CompareDisplayModes);

    GetHardwareCapabilities (renderer, &display_caps);
    if (unk_renderer_init_flag) {
      InitDirect3d (renderer);
    }
  }
  return DD_OK;
}