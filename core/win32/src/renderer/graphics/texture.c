/**
 * @file texture.c
 *
 * Internal graphics texture functions.
 */

#include "graphics_internal.h"

typedef struct {
  // TODO: These are pointers to D3D types.
  DWORD unk1;
  DWORD unk2;
  DWORD unk3;
  DWORD unk4;
  DWORD unk5;
  DWORD unk6;
} UnkTextureStruct;

static UnkTextureStruct textures[512];  // @address 0x00A2DC48

/**
 * Release a texture's Direct3D resources.
 *
 * @address        0x004C1024
 *                 0x004C101C (thunk)
 *
 * @param[in,out]  texture             Texture to release.
 */
static VOID ReleaseUnkTextureStruct (UnkTextureStruct* texture)
{
  // TODO: This function will need to be combed once UnkTextureStruct
  // has been filled with the proper types. The current implementation
  // is a guess based on patterns from other release functions.
  if (texture->unk3) {
    LPDIRECT3DTEXTURE tex;
    (*(*texture->unk4 + 108)) (texture->unk4);  // unknown D3D function call
    texture->unk3->QueryInterface (IID_IDirect3DTexture, (LPVOID*)&tex);
    if (tex) {
      (*(*tex + 28)) (tex);  // unknown D3D function call
      tex->Release ();
    }
    texture->unk3->Release ();
    texture->unk3 = NULL;
  }

  if (texture->unk4) {
    texture->unk4->Release ();
    texture->unk4 = NULL;
  }

  if (texture->unk5) {
    texture->unk5->Release ();
    texture->unk5 = NULL;
  }

  texture->unk1 = NULL;
}

/**
 * Release texture structures.
 *
 * @address        0x00497828
 */
VOID ReleaseUnkTextureStructs ()
{
  for (int i = 0; i < ARRAY_SIZE (textures); i++) {
    if (textures[i].unk1) {
      ReleaseUnkTextureStruct (&textures[i]);  // calls thunk version
    }
  }
}