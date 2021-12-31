/**
 * @file display.c
 *
 * Internal renderer display functions.
 */

#include "renderer_internal.h"

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
HRESULT EnumDisplayModesCallback (LPDDSURFACEDESC2 desc, LPVOID internal)
{
  DisplayCapabilities* caps = (DisplayCapabilities*)internal;

  if (desc->ddpfPixelFormat.dwRGBBitCount == 16) {
    DWORD max_width = unk_renderer_init_flag ? 800 : 640;
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
int CompareDisplayModes (const void* arg1, const void* arg2)
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
HRESULT GetHardwareCapabilities (TmRenderer* renderer, DisplayCapabilities* caps)
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