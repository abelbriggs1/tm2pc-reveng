/**
 * @file texture.c
 *
 * Internal renderer texture functions.
 */

#include <windows.h>

#include "renderer_internal.h"

#include "win32/renderer.h"

#include "texture.h"

/**
 * Release a texture's Direct3D resources.
 *
 * @address        0x004C1024
 *                 0x004C101C (thunk)
 *
 * @param[in,out]  texture             Texture to release.
 */
void ReleaseUnkTextureStruct (UnkTextureStruct* texture)
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