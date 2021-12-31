/**
 * @file display.h
 *
 * Internal renderer display functions and structures.
 */

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

/**
 * Given a DirectDraw surface descriptor which describes a display mode,
 * determine if the display mode is valid and add it to the renderer's
 * list of enumerated display modes if so.
 *
 * This function is a callback for IDirectDraw2_EnumDisplayModes and
 * should not be called by normal means.
 *
 * @param[in]      desc                Surface descriptor which describes a display mode.
 * @param[in,out]  internal            Internal renderer display capabilities structure.
 *
 * @return         DDENUMRET_OK        This function always returns DDENUMRET_OK.
 */
HRESULT EnumDisplayModesCallback (LPDDSURFACEDESC2 desc, LPVOID internal);

/**
 * Given two display modes, compare their internal values to determine
 * if they are identical.
 *
 * This function is a callback for qsort() and should not be called
 * by normal means.
 *
 * @param[in]      arg1                First display mode to compare.
 * @param[in]      arg2                Second display mode to compare.
 *
 * @return         0                   The two display modes are identical.
 * @return         positive            The first display mode is greater.
 * @return         negative            The second display mode is greater.
 */
int CompareDisplayModes (const void* arg1, const void* arg2);

/**
 * Retrieve DirectDraw hardware capabilities and save them in an internal
 * renderer structure for later use.
 *
 * @param[in]      renderer            Renderer context.
 * @param[in,out]  caps                Internal capabilities structure.
 *
 * @return         DD_OK               Hardware capabilities were successfully retrieved.
 * @return         other               An error occurred retrieving hardware capabilities.
 */
HRESULT GetHardwareCapabilities (TmRenderer* renderer, DisplayCapabilities* caps);