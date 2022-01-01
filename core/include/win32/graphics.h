/**
 * @file graphics.h
 *
 * Graphics functions.
 */

#ifndef TM_GRAPHICS_H
#define TM_GRAPHICS_H

typedef struct {
  DWORD display_mode_index;  // unknown type
  LPDIRECTDRAW display_driver;
  LPDIRECTDRAW2 ddraw2;
  LPDIRECTDRAWSURFACE2 primary_surface;
  LPDIRECTDRAWSURFACE2 backbuffer_surface;
  LPDIRECTDRAWSURFACE2 membuffer_surface;
  DWORD unk1;  // unknown 32-bit field
  DWORD unk2;  // unknown 32-bit field
  LPDIRECTDRAWCLIPPER unk_ddclipper;
  LPDIRECT3D2 d3d2;
  LPDIRECT3DDEVICE2 hal_device;
  LPDIRECT3DVIEWPORT2 viewport;
  DWORD unk3;  // unknown pointer to D3D/DDRAW object
  GUID d3d_hal_device_guid;
  LPDIRECT3DMATERIAL2 background;
  LPDIRECT3DMATERIAL2 unk_material;
  D3DMATERIALHANDLE unk_material_handle;
  DWORD graphics_id;
  HWND hwnd;
  DWORD width;
  DWORD height;
  DWORD unk4;                  // unknown 32-bit field
  DWORD maybe_bits_per_pixel;  // unknown type/meaning
  DWORD monitor_freq;
  LPDDBLTFX bltfx;
  DWORD unk_enum_val2;  // unknown 32-bit field
  DWORD unk_enum_val;   // unknown 32-bit field
  RECT rect;
  DWORD maybe_bits_per_pixel2;  // unknown type/meaning
  DWORD unk5;                   // unknown 32-bit field
  DWORD cursor_visible;
} TmGraphics;

/**
 * Initialize the graphics module.
 *
 * @param[in,out]  graphics                  Graphics context.
 * @param[in]      unknown_graphics_flag     TODO: What is this flag?
 *
 * @return         DD_OK                     The graphics was initialized successfully.
 * @return         others                    An error occurred during initialization.
 */
HRESULT TmGraphicsInit (TmGraphics* graphics, DWORD unknown_graphics_flag);

/**
 * Get the current display width.
 *
 * @param[in]      graphics            Graphics context.
 *
 * @return         DWORD               Display width, in pixels.
 */
DWORD TmGraphicsGetDisplayWidth (TmGraphics* graphics);

/**
 * Get the current display height.
 *
 * @param[in]      graphics            Graphics context.
 *
 * @return         DWORD               Display height, in pixels.
 */
DWORD TmGraphicsGetDisplayHeight (TmGraphics* graphics);

/**
 * Get the RGBA bit masks used for the graphics context's draw surface.
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
                                   DWORD* alpha_mask);

/**
 * Set the effects for the graphics context to use on bit block transfers.
 *
 * @param[in,out]  graphics            Graphics context.
 * @param[in]      bltfx               Pointer to DirectDraw BLT effects object.
 */
VOID TmGraphicsSetBltfx (TmGraphics* graphics, LPDDBLTFX bltfx);

/**
 * Get the device context for a DirectDraw surface.
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
                                           LPDIRECTDRAWSURFACE2 surface);

/**
 * Release the device context for a DirectDraw surface.
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
                                               LPDIRECTDRAWSURFACE2 surface);

/**
 * Change display modes and prepare to render frames to the window.
 *
 * On error, this function raises structured exceptions which the caller
 * must handle.
 *
 * @param[in,out]  graphics            Graphics context.
 */
VOID TmGraphicsDisplay (TmGraphics* graphics);

#endif /* TM_GRAPHICS_H */