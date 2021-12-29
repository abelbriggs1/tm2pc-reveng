/**
 * @file renderer.h
 *
 * Renderer functions.
 */

#ifndef TM_RENDERER_H
#define TM_RENDERER_H

typedef struct {
  int display_mode_index;  // unknown type
  LPDIRECTDRAW display_driver;
  LPDIRECTDRAW2 ddraw2;
  LPDIRECTDRAWSURFACE2 primary_surface;
  LPDIRECTDRAWSURFACE2 backbuffer_surface;
  LPDIRECTDRAWSURFACE2 membuffer_surface;
  int unk1;  // unknown 32-bit field
  int unk2;  // unknown 32-bit field
  LPDIRECTDRAWCLIPPER unk_ddclipper;
  LPDIRECT3D2 d3d2;
  LPDIRECT3DDEVICE2 hal_device;
  LPDIRECT3DVIEWPORT2 viewport;
  int unk3;  // unknown pointer to D3D/DDRAW object
  GUID d3d_hal_device_guid;
  LPDIRECT3DMATERIAL2 background;
  LPDIRECT3DMATERIAL2 unk_material;
  D3DMATERIALHANDLE unk_material_handle;
  DWORD renderer_id;
  HWND hwnd;
  int width;
  int height;
  int unk4;                  // unknown 32-bit field
  int maybe_bits_per_pixel;  // unknown type/meaning
  int monitor_freq;
  LPDDBLTFX bltfx;
  int unk_enum_val2;  // unknown 32-bit field
  int unk_enum_val;   // unknown 32-bit field
  RECT rect;
  int maybe_bits_per_pixel2;  // unknown type/meaning
  int unk5;                   // unknown 32-bit field
  int cursor_visible;
} TmRenderer;

/**
 * Initialize the renderer module.
 *
 * @param[in,out]  renderer                  Renderer context.
 * @param[in]      unknown_renderer_flag     TODO: What is this flag?
 *
 * @return         DD_OK                     The renderer was initialized successfully.
 * @return         others                    An error occurred during initialization.
 */
HRESULT TmRendererInit (TmRenderer* renderer, int unknown_renderer_flag);

#endif /* TM_RENDERER_H */