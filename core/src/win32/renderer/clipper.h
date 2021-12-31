/**
 * @file clipper.h
 *
 * Internal renderer clipper functions and structures.
 */

#ifndef TM_CLIPPER_H
#define TM_CLIPPER_H

/**
 * Create the clipper object for the renderer's primary surface. If the
 * clipper already exists, attach it to the primary surface.
 *
 * @param[in,out]  renderer            Renderer context.
 */
VOID CreateClipper (TmRenderer* renderer);

/**
 * Release the clipper object for the primary surface, if it exists.
 *
 * @param[in,out]  renderer            Renderer context.
 */
VOID ReleaseClipper (TmRenderer* renderer);

#endif /* TM_CLIPPER_H */