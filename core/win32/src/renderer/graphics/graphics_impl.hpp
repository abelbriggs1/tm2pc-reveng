/**
 * @file graphics_impl.h
 *
 * Internal implementation of graphics context.
 */

#ifndef TM_GRAPHICS_IMPL_H
#define TM_GRAPHICS_IMPL_H

typedef struct {
  D3DDEVICEDESC hw_desc;
  D3DDEVICEDESC hel_desc;
} DisplayDescriptors;

typedef struct {
  DWORD width;
  DWORD height;
  DWORD rgb_bit_count;
  DWORD monitor_freq;
} DisplayMode;

typedef struct {
  DWORD count;
  DisplayMode modes[30];
  DDCAPS_DX5 hw_caps;
  DDCAPS_DX5 hel_caps;
  DWORD hw_supports_3d;
  DWORD hw_bank_switched;
  DWORD hw_blt_stretched;
  DWORD hw_no_2d_during_3d_scene;
  DisplayDescriptors descriptors;
} DisplayCapabilities;

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

class GraphicsContext {
 public:
  HRESULT Init (DWORD unknown_graphics_flag);
  HRESULT SwapBuffers ();
  HRESULT FillBuffer (DWORD color, LPDIRECTDRAWSURFACE2 surface);
  DWORD GetDisplayWidth ();
  DWORD GetDisplayHeight ();
  VOID GetColorDepthMasks (DWORD* red_mask, DWORD* green_mask, DWORD* blue_mask, DWORD* alpha_mask);
  VOID SetBltfx (LPDDBLTFX bltfx);
  HRESULT GetSurfaceDeviceContext (HDC* hdc, LPDIRECTDRAWSURFACE2 surface);
  HRESULT ReleaseSurfaceDeviceContext (HDC hdc, LPDIRECTDRAWSURFACE2 surface);
  HRESULT SetDisplayMode (DWORD width, DWORD height, DWORD bits_per_pixel, DWORD monitor_freq);

 private:
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
  DWORD unk4;  // unknown 32-bit field
  DWORD bits_per_pixel;
  DWORD monitor_freq;
  LPDDBLTFX bltfx;
  DWORD unk_enum_val2;  // unknown 32-bit field
  DWORD unk_enum_val;   // unknown 32-bit field
  RECT rect;
  DWORD maybe_bits_per_pixel2;  // unknown type/meaning
  DWORD unk5;                   // unknown 32-bit field
  DWORD cursor_visible;

  VOID ReleaseHalDevice ();
  HRESULT CreateBackbufferSurface ();
  HRESULT CreatePrimaryAndMembufferSurfaces ();
  HRESULT InitDirect3d ();
  HRESULT ChangeDisplayMode ();
  VOID AdjustWindow (DWORD left, DWORD top, DWORD width, DWORD height);
  VOID CreateClipper ();
  VOID ReleaseClipper ();
  HRESULT RestoreSurfaces ();
  VOID ReleaseSurfaces ();
  HRESULT CreateSurface (LPDIRECTDRAWSURFACE2 surface,
                         DWORD width,
                         DWORD height,
                         DWORD caps,
                         DWORD back_buffer_count);
  HRESULT GetHardwareCapabilities (DisplayCapabilities* caps);
};

#endif /* TM_GRAPHICS_IMPL_H */