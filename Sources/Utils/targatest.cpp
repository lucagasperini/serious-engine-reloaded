#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#include <Engine/Engine.h>
#include <Engine/Graphics/ImageInfo.h>
#include <Engine/Base/FileName.h>

HWND _hwndMain = NULL; /* FIXME: Cant compile without this global variable */

int main(int argc,char **argv) 
{
   if (argc < 2) {
      fprintf(stderr,"Usage: %s tgafile\n",argv[0]);
      exit(-1);
   }
   CImageInfo iiImageInfo;
   CTFileName *fnPicture = new CTFileName(CTString(argv[1]));
   iiImageInfo.LoadAnyGfxFormat_t( *fnPicture);
    // both dimension must be potentions of 2
    if( (iiImageInfo.ii_Width  == 1<<((int)Log2( (FLOAT)iiImageInfo.ii_Width))) &&
        (iiImageInfo.ii_Height == 1<<((int)Log2( (FLOAT)iiImageInfo.ii_Height))) )
    {
      CTFileName fnTexture = fnPicture->FileDir()+fnPicture->FileName()+".tex";
      // creates new texture with one frame
      CTextureData tdPicture;
      tdPicture.Create_t( &iiImageInfo, iiImageInfo.ii_Width, 1, FALSE);
      tdPicture.Save_t( fnTexture);
    }
}