/**
 * @file graphics.h
 *
 * Public C API for graphics functions.
 */

#ifndef TM_GRAPHICS_H
#define TM_GRAPHICS_H

#define COLOR_MASK_R_15 0x7C00
#define COLOR_MASK_G_15 0x03E0
#define COLOR_MASK_B_15 0x001F

#define COLOR_MASK_R_16 0xF800
#define COLOR_MASK_G_16 0x07E0
#define COLOR_MASK_B_16 0x001F

typedef enum { COLOR_DEPTH_15 = 0, COLOR_DEPTH_16 = 1, COLOR_DEPTH_UNKNOWN = 2 } ColorDepth;

/**
 * Initialize the graphics module.
 *
 * @param[in]      unknown_graphics_flag     TODO: What is this flag?
 *
 * @return         0                         The graphics context was initialized successfully.
 * @return         others                    An error occurred during initialization.
 */
int TmGraphicsInit (int unknown_graphics_flag);

/**
 * Swap the primary and back framebuffers.
 *
 * @return         0                   The framebuffer was swapped successfully.
 * @return         others              An error occurred swapping the framebuffers.
 */
int TmGraphicsSwapFramebuffer ();

/**
 * Fill the primary framebuffer with the specified color.
 *
 * @param[in]      color               16-bit color to fill the framebuffer.
 *
 * @return         0                   The framebuffer was successfully filled.
 * @return         others              An error occurred filling the framebuffer.
 */
int TmGraphicsFillFramebuffer (uint16_t color);

/**
 * Fill all framebuffers with the specified color.
 *
 * @param[in]      color               16-bit color to fill the framebuffers.
 */
void TmGraphicsFillFramebuffers (uint16_t color);

/**
 * Get the current display width.
 *
 * @return         int                 Display width, in pixels.
 */
int TmGraphicsGetDisplayWidth ();

/**
 * Get the current display height.
 *
 * @return         int                 Display height, in pixels.
 */
int TmGraphicsGetDisplayHeight ();

/**
 * Get the RGBA bit masks used for the draw surface.
 *
 * @param[out]     red_mask            Pointer to store red bit mask in. Optional.
 * @param[out]     green_mask          Pointer to store green bit mask in. Optional.
 * @param[out]     blue_mask           Pointer to store blue bit mask in. Optional.
 * @param[out]     alpha_mask          Pointer to store alpha bit mask in. Optional.
 */
void TmGraphicsGetColorDepthMasks (int* red_mask, int* green_mask, int* blue_mask, int* alpha_mask);

#endif /* TM_GRAPHICS_H */